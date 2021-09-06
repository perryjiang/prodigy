#include "movegen/tables.h"

#include <concepts>
#include <utility>

#include "board/direction.h"

namespace prodigy::movegen {
namespace {
template <std::invocable<board::Coordinate> AttackSet>
constexpr board::CoordinateMap<board::Bitboard> non_sliding_attack_table(AttackSet&& attack_set) {
  board::CoordinateMap<board::Bitboard> attack_table;
  board::for_each_coordinate(
      [&](const auto origin) { attack_table[origin] |= std::forward<AttackSet>(attack_set)(origin); });
  return attack_table;
}

template <board::Direction... DIRECTIONS>
constexpr board::Bitboard adjacent_attack_set(const board::Coordinate origin) {
  const auto attack_set = [](const auto target) {
    return target.has_value() ? board::Bitboard(*target) : board::Bitboard();
  };
  return (attack_set(board::directional_offset<DIRECTIONS>(origin)) | ...);
}

template <board::Direction DIRECTION, board::Direction... DIRECTIONS>
constexpr board::Bitboard sliding_attack_set(const board::Coordinate origin, const board::Bitboard occupancy) {
  if constexpr (!sizeof...(DIRECTIONS)) {
    board::Bitboard attack_set;
    for (auto target = board::directional_offset<DIRECTION>(origin); target.has_value();
         target = board::directional_offset<DIRECTION>(*target)) {
      attack_set |= board::Bitboard(*target);
      if (occupancy & board::Bitboard(*target)) {
        break;
      }
    }
    return attack_set;
  } else {
    return sliding_attack_set<DIRECTION>(origin, occupancy) | sliding_attack_set<DIRECTIONS...>(origin, occupancy);
  }
}

constexpr bool same_diagonal_or_antidiagonal(const board::Coordinate origin, const board::Coordinate target) {
  const auto diagonal = [](const auto coordinate) {
    return std::to_underlying(file_of(coordinate)) + std::to_underlying(rank_of(coordinate));
  };
  const auto anti_diagonal = [](const auto coordinate) {
    return std::to_underlying(file_of(coordinate)) - std::to_underlying(rank_of(coordinate));
  };
  return diagonal(origin) == diagonal(target) || anti_diagonal(origin) == anti_diagonal(target);
}
}

Tables::Tables()
    : white_pawn_attack_table_(
          non_sliding_attack_table(adjacent_attack_set<board::Direction::NORTH_EAST, board::Direction::NORTH_WEST>)),
      black_pawn_attack_table_(
          non_sliding_attack_table(adjacent_attack_set<board::Direction::SOUTH_EAST, board::Direction::SOUTH_WEST>)),
      knight_attack_table_(non_sliding_attack_table([](const auto origin) {
        board::Bitboard attack_set;
        if (const auto target = board::directional_offset<board::Direction::NORTH_EAST>(origin); target.has_value()) {
          attack_set |= adjacent_attack_set<board::Direction::NORTH, board::Direction::EAST>(*target);
        }
        if (const auto target = board::directional_offset<board::Direction::SOUTH_EAST>(origin); target.has_value()) {
          attack_set |= adjacent_attack_set<board::Direction::SOUTH, board::Direction::EAST>(*target);
        }
        if (const auto target = board::directional_offset<board::Direction::SOUTH_WEST>(origin); target.has_value()) {
          attack_set |= adjacent_attack_set<board::Direction::SOUTH, board::Direction::WEST>(*target);
        }
        if (const auto target = board::directional_offset<board::Direction::NORTH_WEST>(origin); target.has_value()) {
          attack_set |= adjacent_attack_set<board::Direction::NORTH, board::Direction::WEST>(*target);
        }
        return attack_set;
      })),
      king_attack_table_(non_sliding_attack_table(
          adjacent_attack_set<board::Direction::NORTH, board::Direction::NORTH_EAST, board::Direction::EAST,
                              board::Direction::SOUTH_EAST, board::Direction::SOUTH, board::Direction::SOUTH_WEST,
                              board::Direction::WEST, board::Direction::NORTH_WEST>)),
      bishop_magic_bitboards_(
          [](const auto origin, const auto target) {
            if (file_of(target) == board::File::A || file_of(target) == board::File::H ||
                rank_of(target) == board::Rank::ONE || rank_of(target) == board::Rank::EIGHT) {
              return false;
            }
            return same_diagonal_or_antidiagonal(origin, target);
          },
          sliding_attack_set<board::Direction::NORTH_EAST, board::Direction::SOUTH_EAST, board::Direction::SOUTH_WEST,
                             board::Direction::NORTH_WEST>),
      rook_magic_bitboards_(
          [](const auto origin, const auto target) {
            if (file_of(origin) == file_of(target)) {
              return rank_of(target) != board::Rank::ONE && rank_of(target) != board::Rank::EIGHT;
            }
            if (rank_of(origin) == rank_of(target)) {
              return file_of(target) != board::File::A && file_of(target) != board::File::H;
            }
            return false;
          },
          sliding_attack_set<board::Direction::NORTH, board::Direction::EAST, board::Direction::SOUTH,
                             board::Direction::WEST>),
      ray_table_([&] {
        board::CoordinateMap<board::CoordinateMap<board::Bitboard>> ray_table;
        board::for_each_coordinate([&](const auto origin) {
          board::for_each_coordinate([&](const auto target) {
            if (file_of(origin) == file_of(target) || rank_of(origin) == rank_of(target)) {
              ray_table[origin][target] = (rook_magic_bitboards_.attack_set(origin, board::Bitboard(target)) &
                                           rook_magic_bitboards_.attack_set(target, board::Bitboard(origin))) |
                                          board::Bitboard(target);
            } else if (same_diagonal_or_antidiagonal(origin, target)) {
              ray_table[origin][target] = (bishop_magic_bitboards_.attack_set(origin, board::Bitboard(target)) &
                                           bishop_magic_bitboards_.attack_set(target, board::Bitboard(origin))) |
                                          board::Bitboard(target);
            }
          });
        });
        return ray_table;
      }()) {}

board::Bitboard Tables::ray(const board::Coordinate origin, const board::Coordinate target) const {
  return ray_table_[origin][target];
}
}
