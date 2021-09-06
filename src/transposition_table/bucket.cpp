#include "transposition_table/bucket.h"

#include <boost/assert.hpp>
#include <tuple>

#include "base/fill_array.h"
#include "board/coordinate.h"
#include "board/move.h"
#include "search/node_type.h"

namespace prodigy::transposition_table {
Bucket::Bucket()
    : entries_(fill_array<decltype(entries_)::value_type, std::tuple_size_v<decltype(entries_)>>(
          0ULL, Value(board::Move(board::Coordinate::A1, board::Coordinate::A1), 0, 0, search::NodeType::PV, 0))) {}

std::optional<Value> Bucket::find(const Key key, const Ply generation) {
  for (auto& entry : entries_) {
    if (const auto [candidate_key, candidate_value] = entry.load();
        candidate_value.depth() != 0 && key == candidate_key) {
      if (generation == candidate_value.generation()) {
        return candidate_value;
      }
      const Value value(candidate_value.move(), candidate_value.score(), candidate_value.depth(),
                        candidate_value.node_type(), generation);
      entry.store(key, value);
      return value;
    }
  }
  return std::nullopt;
}

bool Bucket::try_insert(const Key key, const Value value) {
  BOOST_ASSERT(value.depth());
  auto* least_valuable_entry = &entries_.front();
  auto least_valuable_value = least_valuable_entry->load().second;
  for (auto& entry : entries_) {
    const auto [stored_key, stored_value] = entry.load();
    if (stored_value.depth() == 0) {
      entry.store(key, value);
      return true;
    }
    if (key == stored_key) {
      const auto replace = value.depth() > stored_value.depth();
      if (replace) {
        entry.store(key, value);
      }
      return replace;
    }
    if (less_valuable_than(stored_value, least_valuable_value, value.generation())) {
      least_valuable_entry = &entry;
      least_valuable_value = stored_value;
    }
  }
  least_valuable_entry->store(key, value);
  return true;
}
}
