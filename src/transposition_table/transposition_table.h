#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "base/ply.h"
#include "board/move.h"
#include "eval/score.h"
#include "search/node_type.h"
#include "transposition_table/bucket.h"
#include "transposition_table/key.h"
#include "transposition_table/value.h"

namespace prodigy::transposition_table {
class TranspositionTable final {
 public:
  explicit TranspositionTable(std::size_t megabytes = 16);

  TranspositionTable(const TranspositionTable&) = delete;
  TranspositionTable& operator=(const TranspositionTable&) = delete;

  TranspositionTable(TranspositionTable&&) = delete;
  TranspositionTable& operator=(TranspositionTable&&) = delete;

  [[nodiscard]] std::optional<Value> find(Key);
  bool try_insert(Key, board::Move, eval::Score, Ply depth, search::NodeType);
  void advance_generation();

 private:
  std::vector<Bucket> buckets_;

  // TODO: does this need to be atomic?
  Ply generation_ = 0;
};
}
