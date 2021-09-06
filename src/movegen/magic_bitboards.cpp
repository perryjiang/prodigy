#include "movegen/magic_bitboards.h"

#include <algorithm>
#include <bit>
#include <concepts>
#include <tuple>
#include <utility>

#include "base/uniform_distribution.h"

namespace prodigy::movegen {
namespace {
std::vector<board::Bitboard> generate_subsets(const board::Bitboard bitboard) {
  if (!bitboard) {
    return {board::Bitboard()};
  }
  std::vector<board::Bitboard> subsets;
  for (const auto lsb = bitboard.lsb(); const auto subset : generate_subsets(bitboard & ~lsb)) {
    subsets.push_back(subset | lsb);
    subsets.push_back(subset);
  }
  return subsets;
}

Magic random_sparse_magic() {
  return uniform_distribution<Magic>() & uniform_distribution<Magic>() & uniform_distribution<Magic>();
}

constexpr AttackTable::size_type attack_table_index(const board::Bitboard occupancy, const board::Bitboard mask,
                                                    const Magic magic, const std::uint8_t shift) {
  return (occupancy & mask).underlying() * magic >> shift;
}

template <std::invocable<board::Bitboard> AttackSet>
std::tuple<Magic, std::uint8_t, AttackTable> find_magic(const board::Bitboard mask, AttackSet&& attack_set) {
  std::vector<std::pair<board::Bitboard, board::Bitboard>> occupancy_to_attack_set;
  for (const auto occupancy : generate_subsets(mask)) {
    occupancy_to_attack_set.emplace_back(occupancy, std::forward<AttackSet>(attack_set)(occupancy));
  }

  static constexpr auto INVALID_ATTACK_SET = ~board::Bitboard();
  std::tuple result(random_sparse_magic(), (~mask).popcount(),
                    AttackTable(1ULL << mask.popcount(), INVALID_ATTACK_SET));
  for (auto& [magic, shift, attack_table] = result;;
       magic = random_sparse_magic(), std::fill(attack_table.begin(), attack_table.end(), INVALID_ATTACK_SET)) {
    while (std::popcount(attack_table_index(mask, mask, magic, shift)) < mask.popcount() * 2 / 3) {
      magic = random_sparse_magic();
    }
    auto collision = false;
    for (const auto& [occupancy, attack_set] : occupancy_to_attack_set) {
      auto& attack_table_entry = attack_table[attack_table_index(occupancy, mask, magic, shift)];
      if (attack_table_entry != INVALID_ATTACK_SET && attack_table_entry != attack_set) {
        collision = true;
        break;
      }
      attack_table_entry = attack_set;
    }
    if (!collision) {
      return result;
    }
  }
}
}

MagicBitboards::MagicBitboards(
    const std::function<bool(board::Coordinate origin, board::Coordinate target)>& mask_contains,
    const std::function<board::Bitboard(board::Coordinate origin, board::Bitboard occupancy)>& attack_set)
    : origin_to_record_([&] {
        board::CoordinateMap<Record> origin_to_record;
        board::for_each_coordinate([&](const auto origin) {
          auto& [mask, magic, shift, attack_table] = origin_to_record[origin];
          board::for_each_coordinate([&](const auto target) {
            if (target != origin && mask_contains(origin, target)) {
              mask |= board::Bitboard(target);
            }
          });
          std::tie(magic, shift, attack_table) =
              find_magic(mask, [&](const auto occupancy) { return attack_set(origin, occupancy); });
        });
        return origin_to_record;
      }()) {}

board::Bitboard MagicBitboards::attack_set(const board::Coordinate origin, const board::Bitboard occupancy) const {
  const auto& [mask, magic, shift, attack_table] = origin_to_record_[origin];
  return attack_table[attack_table_index(occupancy, mask, magic, shift)];
}
}
