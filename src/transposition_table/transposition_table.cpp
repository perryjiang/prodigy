#include "transposition_table/transposition_table.h"

namespace prodigy::transposition_table {
TranspositionTable::TranspositionTable(const std::size_t megabytes)
    : buckets_(megabytes * (1 << 20) / sizeof(Bucket)) {}

std::optional<Value> TranspositionTable::find(const Key key) {
  return buckets_[key % buckets_.size()].find(key, generation_);
}

bool TranspositionTable::try_insert(const Key key, const board::Move move, const eval::Score score, const Ply depth,
                                    const search::NodeType node_type) {
  return buckets_[key % buckets_.size()].try_insert(key, Value(move, score, depth, node_type, generation_));
}

void TranspositionTable::advance_generation() { ++generation_; }
}
