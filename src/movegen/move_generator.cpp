#include "movegen/move_generator.h"

#include <boost/assert.hpp>

#include "board/color_traits.h"
#include "board/coordinate_map.h"
#include "board/direction.h"

namespace prodigy::movegen {
template <board::Color COLOR, board::PieceType PIECE_TYPE>
inline board::Bitboard MoveGenerator::pseudo_legal_move_set(const board::Position& position,
                                                            const board::Coordinate origin,
                                                            const board::Bitboard occupancy) const {
  if constexpr (PIECE_TYPE == board::PieceType::PAWN) {
    using ColorTraits = board::ColorTraits<COLOR>;
    const auto single_push_target = board::unsafe_directional_offset<ColorTraits::RELATIVE_NORTH>(origin);
    auto push_set = board::Bitboard(single_push_target) & ~occupancy;
    if (push_set && rank_of(origin) == ColorTraits::PAWN_INITIAL_RANK) {
      if (const auto double_push_target = board::directional_offset<ColorTraits::RELATIVE_NORTH>(single_push_target);
          double_push_target.has_value()) {
        push_set |= board::Bitboard(*double_push_target) & ~occupancy;
      }
    }
    return (tables_.attack_set<COLOR, PIECE_TYPE>(origin, occupancy) & position.all_pieces<~COLOR>()) | push_set;
  } else {
    return tables_.attack_set<COLOR, PIECE_TYPE>(origin, occupancy) & ~position.all_pieces<COLOR>();
  }
}

template <board::Color ACTIVE_COLOR>
MoveList MoveGenerator::generate(const board::Position& position) const {
  using ActiveColorTraits = board::ColorTraits<ACTIVE_COLOR>;
  MoveList move_list;
  const auto occupancy = position.all_pieces<ACTIVE_COLOR>() | position.all_pieces<~ACTIVE_COLOR>();
  const auto king_coordinate = unsafe_to_coordinate(position.pieces<ACTIVE_COLOR, board::PieceType::KING>());
  const auto king_danger_set = [&] {
    const auto attack_set =
        [&, active_kingless_occupancy =
                occupancy & ~position.pieces<ACTIVE_COLOR, board::PieceType::KING>()]<board::PieceType PIECE_TYPE> {
          board::Bitboard attack_set;
          board::for_each_coordinate(position.pieces<~ACTIVE_COLOR, PIECE_TYPE>(), [&](const auto origin) {
            attack_set |= tables_.attack_set<~ACTIVE_COLOR, PIECE_TYPE>(origin, active_kingless_occupancy);
          });
          return attack_set;
        };
    return attack_set.template operator()<board::PieceType::PAWN>() |
           attack_set.template operator()<board::PieceType::KNIGHT>() |
           attack_set.template operator()<board::PieceType::BISHOP>() |
           attack_set.template operator()<board::PieceType::ROOK>() |
           attack_set.template operator()<board::PieceType::QUEEN>() |
           attack_set.template operator()<board::PieceType::KING>();
  }();
  const auto king_attacker_set = [&] {
    const auto attacker_set = [&]<board::PieceType PIECE_TYPE> {
      return tables_.attack_set<ACTIVE_COLOR, PIECE_TYPE>(king_coordinate, occupancy) &
             position.pieces<~ACTIVE_COLOR, PIECE_TYPE>();
    };
    return attacker_set.template operator()<board::PieceType::PAWN>() |
           attacker_set.template operator()<board::PieceType::KNIGHT>() |
           attacker_set.template operator()<board::PieceType::BISHOP>() |
           attacker_set.template operator()<board::PieceType::ROOK>() |
           attacker_set.template operator()<board::PieceType::QUEEN>();
  }();
  const auto king_attacker_count = king_attacker_set.popcount();
  BOOST_ASSERT(king_attacker_count <= 2);
  for_each_coordinate(
      pseudo_legal_move_set<ACTIVE_COLOR, board::PieceType::KING>(position, king_coordinate, occupancy) &
          ~king_danger_set,
      [&](const auto target) { move_list.emplace_back(king_coordinate, target); });
  if (king_attacker_count == 2) {
    return move_list;
  }
  const auto check_evasion_mask =
      king_attacker_count ? king_attacker_set | tables_.ray(king_coordinate, unsafe_to_coordinate(king_attacker_set))
                          : ~board::Bitboard();
  const auto pinned_move_restriction_masks = [&] {
    const auto xray_attack_set = [&]<board::PieceType PIECE_TYPE> {
      const auto attack_set = tables_.attack_set<~ACTIVE_COLOR, PIECE_TYPE>(king_coordinate, occupancy);
      const auto blockers = position.all_pieces<ACTIVE_COLOR>() & attack_set;
      return attack_set ^ tables_.attack_set<~ACTIVE_COLOR, PIECE_TYPE>(king_coordinate, occupancy ^ blockers);
    };
    const auto pinner_set = (xray_attack_set.template operator()<board::PieceType::BISHOP>() &
                             position.pieces<~ACTIVE_COLOR, board::PieceType::BISHOP, board::PieceType::QUEEN>()) |
                            (xray_attack_set.template operator()<board::PieceType::ROOK>() &
                             position.pieces<~ACTIVE_COLOR, board::PieceType::ROOK, board::PieceType::QUEEN>());
    auto pinned_move_restriction_masks = board::CoordinateMap<board::Bitboard>::fill(~board::Bitboard());
    for_each_coordinate(pinner_set, [&](const auto pinner) {
      const auto ray = tables_.ray(king_coordinate, pinner);
      pinned_move_restriction_masks[unsafe_to_coordinate(ray & position.all_pieces<ACTIVE_COLOR>())] = ray;
    });
    return pinned_move_restriction_masks;
  }();
  const auto generate_legal_moves = [&]<board::PieceType PIECE_TYPE> {
    static_assert(PIECE_TYPE != board::PieceType::PAWN && PIECE_TYPE != board::PieceType::KING);
    for_each_coordinate(position.pieces<ACTIVE_COLOR, PIECE_TYPE>(), [&](const auto origin) {
      for_each_coordinate(pseudo_legal_move_set<ACTIVE_COLOR, PIECE_TYPE>(position, origin, occupancy) &
                              check_evasion_mask & pinned_move_restriction_masks[origin],
                          [&](const auto target) { move_list.emplace_back(origin, target); });
    });
  };
  generate_legal_moves.template operator()<board::PieceType::KNIGHT>();
  generate_legal_moves.template operator()<board::PieceType::BISHOP>();
  generate_legal_moves.template operator()<board::PieceType::ROOK>();
  generate_legal_moves.template operator()<board::PieceType::QUEEN>();
  for_each_bit(position.pieces<ACTIVE_COLOR, board::PieceType::PAWN>(), [&](const auto pawn) {
    const auto origin = unsafe_to_coordinate(pawn);
    for_each_coordinate(pseudo_legal_move_set<ACTIVE_COLOR, board::PieceType::PAWN>(position, origin, occupancy) &
                            check_evasion_mask & pinned_move_restriction_masks[origin],
                        [&](const auto target) {
                          if (rank_of(target) == ActiveColorTraits::PAWN_PROMOTION_RANK) {
                            for (const auto promotion : {board::PieceType::KNIGHT, board::PieceType::BISHOP,
                                                         board::PieceType::ROOK, board::PieceType::QUEEN}) {
                              move_list.emplace_back(origin, target, promotion);
                            }
                          } else {
                            move_list.emplace_back(origin, target);
                          }
                        });
    if (const auto en_passant_target = position.en_passant_target(); en_passant_target.has_value()) {
      board::Bitboard en_passant_target_mask(*en_passant_target);
      if (tables_.ray(origin, *en_passant_target) == en_passant_target_mask &&
          rank_of(origin) == ActiveColorTraits::EN_PASSANT_CAPTURE_ORIGIN_RANK) {
        BOOST_ASSERT(!(occupancy & board::Bitboard(*en_passant_target)));
        BOOST_ASSERT(rank_of(*en_passant_target) == board::ColorTraits<~ACTIVE_COLOR>::EN_PASSANT_TARGET_RANK);
        const auto en_passant_target_pawn =
            board::Bitboard(board::unsafe_directional_offset<ActiveColorTraits::RELATIVE_SOUTH>(*en_passant_target));
        BOOST_ASSERT(position.all_pieces<~ACTIVE_COLOR>() & en_passant_target_pawn);
        const auto occupancy_after_en_passant = (occupancy ^ pawn ^ en_passant_target_pawn) | en_passant_target_mask;
        if (const auto pinning_bishops_or_queens =
                tables_.attack_set<~ACTIVE_COLOR, board::PieceType::BISHOP>(king_coordinate,
                                                                            occupancy_after_en_passant) &
                position.pieces<~ACTIVE_COLOR, board::PieceType::BISHOP, board::PieceType::QUEEN>();
            !pinning_bishops_or_queens) {
          if (const auto pinning_rooks_or_queens =
                  tables_.attack_set<~ACTIVE_COLOR, board::PieceType::ROOK>(king_coordinate,
                                                                            occupancy_after_en_passant) &
                  position.pieces<~ACTIVE_COLOR, board::PieceType::ROOK, board::PieceType::QUEEN>();
              !pinning_rooks_or_queens) {
            if (en_passant_target_mask & check_evasion_mask || en_passant_target_pawn & check_evasion_mask) {
              move_list.emplace_back(origin, *en_passant_target);
            }
          }
        }
      }
    }
  });
  if (king_attacker_count == 0) {
    const auto maybe_generate_castle = [&](const auto king_origin, const auto king_target, const auto rook_origin) {
      if ((tables_.ray(king_origin, rook_origin) & occupancy) == board::Bitboard(rook_origin) &&
          !(king_danger_set & tables_.ray(king_origin, king_target))) {
        move_list.emplace_back(king_origin, king_target);
      }
    };
    if ((position.castling_rights() & ActiveColorTraits::KINGSIDE_CASTLING_RIGHTS) != board::CastlingRights::NONE) {
      maybe_generate_castle(ActiveColorTraits::KING_INITIAL_ORIGIN, ActiveColorTraits::KING_KINGSIDE_CASTLE_TARGET,
                            ActiveColorTraits::KINGSIDE_ROOK_INITIAL_ORIGIN);
    }
    if ((position.castling_rights() & ActiveColorTraits::QUEENSIDE_CASTLING_RIGHTS) != board::CastlingRights::NONE) {
      maybe_generate_castle(ActiveColorTraits::KING_INITIAL_ORIGIN, ActiveColorTraits::KING_QUEENSIDE_CASTLE_TARGET,
                            ActiveColorTraits::QUEENSIDE_ROOK_INITIAL_ORIGIN);
    }
  }
  return move_list;
}

template MoveList MoveGenerator::generate<board::Color::WHITE>(const board::Position&) const;
template MoveList MoveGenerator::generate<board::Color::BLACK>(const board::Position&) const;
}
