#include "board/position.h"

#include <boost/assert.hpp>
#include <cctype>
#include <ostream>
#include <sstream>
#include <utility>

#include "base/string_utils.h"
#include "board/color_traits.h"
#include "board/direction.h"
#include "board/file.h"
#include "board/rank.h"
#include "board/starting_position_fen.h"
#include "zobrist/randoms.h"

namespace prodigy::board {
Position Position::from_fen(const std::string_view fen) {
  const auto [piece_placement, active_color, castling_rights, en_passant_target, halfmove_clock, fullmove_number] =
      split<6>(fen);
  return Position(
      [&] {
        Board board;
        for (auto rank_start_coordinate = Coordinate::A8; const auto rank_contents : split<8>(piece_placement, "/")) {
          BOOST_ASSERT(!piece_placement.empty());
          for (auto current_coordinate = rank_start_coordinate; const auto placement : rank_contents) {
            const auto try_advance = [&] {
              if (file_of(current_coordinate) != File::H) {
                current_coordinate = unsafe_directional_offset<Direction::EAST>(current_coordinate);
              }
            };
            if (std::isdigit(placement)) {
              for (auto empty_count = placement - '0'; empty_count; --empty_count) {
                try_advance();
              }
            } else {
              switch (const auto mask = Bitboard(current_coordinate); placement) {
#define _(CHAR, COLOR, PIECE_TYPE)                            \
  case CHAR:                                                  \
    board.get<Color::COLOR, PieceType::PIECE_TYPE>() |= mask; \
    break
                _('P', WHITE, PAWN);
                _('N', WHITE, KNIGHT);
                _('B', WHITE, BISHOP);
                _('R', WHITE, ROOK);
                _('Q', WHITE, QUEEN);
                _('K', WHITE, KING);
                _('p', BLACK, PAWN);
                _('n', BLACK, KNIGHT);
                _('b', BLACK, BISHOP);
                _('r', BLACK, ROOK);
                _('q', BLACK, QUEEN);
                _('k', BLACK, KING);
#undef _
                default:
                  BOOST_ASSERT(false);
                  __builtin_unreachable();
              }
              try_advance();
            }
          }
          if (rank_of(rank_start_coordinate) != Rank::ONE) {
            rank_start_coordinate = unsafe_directional_offset<Direction::SOUTH>(rank_start_coordinate);
          }
        }
        return board;
      }(),
      [&] {
        BOOST_ASSERT(active_color.size() == 1);
        return to_color(active_color[0]);
      }(),
      to_castling_rights(castling_rights),
      [&] -> std::optional<Coordinate> {
        if (en_passant_target == "-") {
          return std::nullopt;
        }
        BOOST_ASSERT(en_passant_target.size() == 2);
        return to_coordinate(to_file(en_passant_target[0]), to_rank(en_passant_target[1]));
      }(),
      unsafe_to_arithmetic<decltype(halfmove_clock_)>(halfmove_clock),
      unsafe_to_arithmetic<decltype(fullmove_number_)>(fullmove_number));
}

const Position& Position::starting_position() {
  static const auto STARTING_POSITION = Position::from_fen(STARTING_POSITION_FEN);
  return STARTING_POSITION;
}

std::string Position::fen() const {
  std::ostringstream fen;
  for (const auto rank :
       {Rank::EIGHT, Rank::SEVEN, Rank::SIX, Rank::FIVE, Rank::FOUR, Rank::THREE, Rank::TWO, Rank::ONE}) {
    auto empty_coordinates = 0UZ;
    for (const auto file : {File::A, File::B, File::C, File::D, File::E, File::F, File::G, File::H}) {
#define _(COLOR, PIECE_TYPE, CHAR)                                \
  if (const auto mask = Bitboard(to_coordinate(file, rank));      \
      board_.get<Color::COLOR, PieceType::PIECE_TYPE>() & mask) { \
    if (empty_coordinates != 0) {                                 \
      fen << empty_coordinates;                                   \
      empty_coordinates = 0;                                      \
    }                                                             \
    fen << CHAR;                                                  \
    continue;                                                     \
  }
      _(WHITE, PAWN, 'P');
      _(WHITE, KNIGHT, 'N');
      _(WHITE, BISHOP, 'B');
      _(WHITE, ROOK, 'R');
      _(WHITE, QUEEN, 'Q');
      _(WHITE, KING, 'K');
      _(BLACK, PAWN, 'p');
      _(BLACK, KNIGHT, 'n');
      _(BLACK, BISHOP, 'b');
      _(BLACK, ROOK, 'r');
      _(BLACK, QUEEN, 'q');
      _(BLACK, KING, 'k');
#undef _

      ++empty_coordinates;
    }
    if (empty_coordinates != 0) {
      fen << empty_coordinates;
    }
    if (rank != Rank::ONE) {
      fen << '/';
    }
  }
  fen << ' ' << active_color_ << ' ' << castling_rights_ << ' ';
  if (en_passant_target_.has_value()) {
    fen << *en_passant_target_;
  } else {
    fen << '-';
  }
  fen << ' ' << static_cast<std::size_t>(halfmove_clock_) << ' ' << fullmove_number_;
  return std::move(fen).str();
}

std::ostream& operator<<(std::ostream& os, const Position& position) {
  static constexpr auto FILE_LABELS = "     a   b   c   d   e   f   g   h\n";
  static constexpr auto RANK_SEPARATOR = "   +---+---+---+---+---+---+---+---+\n";
  os << FILE_LABELS << RANK_SEPARATOR;
  for (const auto rank : {'8', '7', '6', '5', '4', '3', '2', '1'}) {
    os << ' ' << rank << " |";
    for (const auto file : {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}) {
      os << ' ' << [&] {
#define _(COLOR, PIECE_TYPE, REPR)                                               \
  do {                                                                           \
    if (const auto mask = Bitboard(to_coordinate(to_file(file), to_rank(rank))); \
        position.pieces<Color::COLOR, PieceType::PIECE_TYPE>() & mask) {         \
      return REPR;                                                               \
    }                                                                            \
  } while (false)
        _(WHITE, PAWN, "♙");
        _(WHITE, KNIGHT, "♘");
        _(WHITE, BISHOP, "♗");
        _(WHITE, ROOK, "♖");
        _(WHITE, QUEEN, "♕");
        _(WHITE, KING, "♔");
        _(BLACK, PAWN, "♟︎");
        _(BLACK, KNIGHT, "♞");
        _(BLACK, BISHOP, "♝");
        _(BLACK, ROOK, "♜");
        _(BLACK, QUEEN, "♛");
        _(BLACK, KING, "♚");
#undef _
        return " ";
      }() << " |";
    }
    os << ' ' << rank << '\n' << RANK_SEPARATOR;
  }
  os << FILE_LABELS << '\n';
  os << " Active color   : " << position.active_color() << '\n';
  os << " Castling rights: " << position.castling_rights() << '\n';
  if (position.en_passant_target().has_value()) {
    os << " En passant     : " << *position.en_passant_target() << '\n';
  } else {
    os << " En passant     : -\n";
  }
  os << " Halfmove clock : " << static_cast<std::size_t>(position.halfmove_clock()) << '\n';
  os << " Fullmove number: " << position.fullmove_number() << '\n';
  os << " FEN            : " << position.fen() << '\n';
  os << " Hash           : " << position.hash();
  return os;
}

Position::Position(const Board& board, const Color active_color, const CastlingRights castling_rights,
                   const std::optional<Coordinate> en_passant_target, const Ply halfmove_clock,
                   const std::uint16_t fullmove_number)
    : Position(board, active_color, castling_rights, en_passant_target, halfmove_clock, fullmove_number, [&] {
        const auto& zobrist_randoms = zobrist::Randoms::instance();
        zobrist::Hash hash = 0;
#define _(COLOR, PIECE_TYPE)                                                                         \
  for_each_coordinate(board.get<Color::COLOR, PieceType::PIECE_TYPE>(), [&](const auto coordinate) { \
    hash ^= zobrist_randoms.coordinate_random<Color::COLOR, PieceType::PIECE_TYPE>(coordinate);      \
  })
        _(WHITE, PAWN);
        _(WHITE, KNIGHT);
        _(WHITE, BISHOP);
        _(WHITE, ROOK);
        _(WHITE, QUEEN);
        _(WHITE, KING);
        _(BLACK, PAWN);
        _(BLACK, KNIGHT);
        _(BLACK, BISHOP);
        _(BLACK, ROOK);
        _(BLACK, QUEEN);
        _(BLACK, KING);
#undef _
        if (active_color == Color::WHITE) {
          hash ^= zobrist_randoms.active_color_random();
        }
        hash ^= zobrist_randoms.castling_rights_random(castling_rights);
        if (en_passant_target.has_value()) {
          hash ^= zobrist_randoms.en_passant_file_random(file_of(*en_passant_target));
        }
        return hash;
      }()) {
}

template <Color ACTIVE_COLOR>
Position Position::apply(const Move move) const {
  BOOST_ASSERT(ACTIVE_COLOR == active_color_);
  using ActiveColorTraits = ColorTraits<ACTIVE_COLOR>;
  const auto& zobrist_randoms = zobrist::Randoms::instance();
  auto board = board_;
  auto castling_rights = castling_rights_;
  std::optional<Coordinate> en_passant_target;
  auto halfmove_clock = halfmove_clock_ + 1;
  auto hash =
      hash_ ^ zobrist_randoms.active_color_random() ^
      (en_passant_target_.has_value() ? zobrist_randoms.en_passant_file_random(file_of(*en_passant_target_)) : 0);
  const Bitboard origin_mask(move.origin());
  const Bitboard target_mask(move.target());
  const auto toggle_piece = [&]<Color COLOR, PieceType PIECE_TYPE>(const Bitboard mask, const Coordinate coordinate) {
    board.get<COLOR, PIECE_TYPE>() ^= mask;
    hash ^= zobrist_randoms.coordinate_random<COLOR, PIECE_TYPE>(coordinate);
  };
  const auto non_pawn_move = [&]<PieceType PIECE_TYPE> {
    toggle_piece.template operator()<ACTIVE_COLOR, PIECE_TYPE>(origin_mask, move.origin());
    toggle_piece.template operator()<ACTIVE_COLOR, PIECE_TYPE>(target_mask, move.target());
  };
  const auto revoke_castling_rights = [&]<CastlingRights CASTLING_RIGHTS> {
    hash ^= zobrist_randoms.castling_rights_random(castling_rights);
    castling_rights &= ~CASTLING_RIGHTS;
    hash ^= zobrist_randoms.castling_rights_random(castling_rights);
  };
  const auto origin_piece_type = piece_type_at<ACTIVE_COLOR>(origin_mask);
  BOOST_ASSERT(origin_piece_type.has_value());
  switch (*origin_piece_type) {
    case PieceType::PAWN:
      halfmove_clock = 0;
      toggle_piece.template operator()<ACTIVE_COLOR, PieceType::PAWN>(origin_mask, move.origin());
      if (const auto promotion = move.promotion(); promotion.has_value()) {
        switch (*promotion) {
#define _(PIECE_TYPE)                                                                                  \
  case PieceType::PIECE_TYPE:                                                                          \
    toggle_piece.template operator()<ACTIVE_COLOR, PieceType::PIECE_TYPE>(target_mask, move.target()); \
    break
          _(KNIGHT);
          _(BISHOP);
          _(ROOK);
          _(QUEEN);
#undef _
          default:
            BOOST_ASSERT(false);
            __builtin_unreachable();
        }
        break;
      }
      toggle_piece.template operator()<ACTIVE_COLOR, PieceType::PAWN>(target_mask, move.target());
      if (rank_of(move.origin()) == ActiveColorTraits::PAWN_INITIAL_RANK) {
        if (rank_of(move.target()) == ActiveColorTraits::PAWN_DOUBLE_PUSH_TARGET_RANK) {
          const auto file = file_of(move.target());
          en_passant_target = to_coordinate(file, ActiveColorTraits::EN_PASSANT_TARGET_RANK);
          hash ^= zobrist_randoms.en_passant_file_random(file);
        }
      } else if (en_passant_target_ == move.target()) {
        const auto coordinate = unsafe_directional_offset<ActiveColorTraits::RELATIVE_SOUTH>(*en_passant_target_);
        toggle_piece.template operator()<~ACTIVE_COLOR, PieceType::PAWN>(Bitboard(coordinate), coordinate);
      }
      break;
    case PieceType::ROOK:
      non_pawn_move.template operator()<PieceType::ROOK>();
      if (move.origin() == ActiveColorTraits::KINGSIDE_ROOK_INITIAL_ORIGIN) {
        revoke_castling_rights.template operator()<ActiveColorTraits::KINGSIDE_CASTLING_RIGHTS>();
      } else if (move.origin() == ActiveColorTraits::QUEENSIDE_ROOK_INITIAL_ORIGIN) {
        revoke_castling_rights.template operator()<ActiveColorTraits::QUEENSIDE_CASTLING_RIGHTS>();
      }
      break;
    case PieceType::KING: {
      non_pawn_move.template operator()<PieceType::KING>();
      const auto move_rook = [&]<Coordinate ORIGIN, Coordinate TARGET> {
        toggle_piece.template operator()<ACTIVE_COLOR, PieceType::ROOK>(Bitboard(ORIGIN), ORIGIN);
        toggle_piece.template operator()<ACTIVE_COLOR, PieceType::ROOK>(Bitboard(TARGET), TARGET);
      };
      if ((castling_rights & ActiveColorTraits::KINGSIDE_CASTLING_RIGHTS) != CastlingRights::NONE &&
          move.target() == ActiveColorTraits::KING_KINGSIDE_CASTLE_TARGET) {
        move_rook.template
        operator()<ActiveColorTraits::KINGSIDE_ROOK_INITIAL_ORIGIN, ActiveColorTraits::KINGSIDE_ROOK_CASTLE_TARGET>();
      } else if ((castling_rights & ActiveColorTraits::QUEENSIDE_CASTLING_RIGHTS) != CastlingRights::NONE &&
                 move.target() == ActiveColorTraits::KING_QUEENSIDE_CASTLE_TARGET) {
        move_rook.template
        operator()<ActiveColorTraits::QUEENSIDE_ROOK_INITIAL_ORIGIN, ActiveColorTraits::QUEENSIDE_ROOK_CASTLE_TARGET>();
      }
      revoke_castling_rights.template
      operator()<ActiveColorTraits::KINGSIDE_CASTLING_RIGHTS | ActiveColorTraits::QUEENSIDE_CASTLING_RIGHTS>();
      break;
    }
#define _(PIECE_TYPE)                                           \
  case PieceType::PIECE_TYPE:                                   \
    non_pawn_move.template operator()<PieceType::PIECE_TYPE>(); \
    break
      _(KNIGHT);
      _(BISHOP);
      _(QUEEN);
#undef _
  }
  if (const auto target_piece_type = piece_type_at<~ACTIVE_COLOR>(target_mask); target_piece_type.has_value()) {
    halfmove_clock = 0;
    switch (*target_piece_type) {
      case PieceType::ROOK: {
        toggle_piece.template operator()<~ACTIVE_COLOR, PieceType::ROOK>(target_mask, move.target());
        using OtherColorTraits = ColorTraits<~ACTIVE_COLOR>;
        if (move.target() == OtherColorTraits::KINGSIDE_ROOK_INITIAL_ORIGIN) {
          revoke_castling_rights.template operator()<OtherColorTraits::KINGSIDE_CASTLING_RIGHTS>();
        } else if (move.target() == OtherColorTraits::QUEENSIDE_ROOK_INITIAL_ORIGIN) {
          revoke_castling_rights.template operator()<OtherColorTraits::QUEENSIDE_CASTLING_RIGHTS>();
        }
      } break;
#define _(PIECE_TYPE)                                                                                   \
  case PieceType::PIECE_TYPE:                                                                           \
    toggle_piece.template operator()<~ACTIVE_COLOR, PieceType::PIECE_TYPE>(target_mask, move.target()); \
    break
        _(PAWN);
        _(KNIGHT);
        _(BISHOP);
        _(QUEEN);
#undef _
      case PieceType::KING:
        BOOST_ASSERT(false);
        __builtin_unreachable();
    }
  }
  BOOST_ASSERT(hash == Position(board, ~ACTIVE_COLOR, castling_rights, en_passant_target, halfmove_clock,
                                fullmove_number_ + (ACTIVE_COLOR == Color::BLACK))
                           .hash());
  return Position(board, ~ACTIVE_COLOR, castling_rights, en_passant_target, halfmove_clock,
                  fullmove_number_ + (ACTIVE_COLOR == Color::BLACK), hash);
}

template Position Position::apply<Color::WHITE>(Move) const;
template Position Position::apply<Color::BLACK>(Move) const;
}
