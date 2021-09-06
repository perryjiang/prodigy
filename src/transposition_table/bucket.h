#pragma once

#include <array>
#include <optional>

#include "base/ply.h"
#include "base/cache_line_size.h"
#include "transposition_table/entry.h"
#include "transposition_table/key.h"
#include "transposition_table/value.h"

namespace prodigy::transposition_table {
class alignas(CACHE_LINE_SIZE) Bucket final {
 public:
  static_assert(CACHE_LINE_SIZE % sizeof(Entry) == 0);
  static constexpr auto BUCKET_SIZE = CACHE_LINE_SIZE / sizeof(Entry);

  Bucket();

  std::optional<Value> find(Key, Ply generation);
  bool try_insert(Key, Value);

 private:
  std::array<Entry, BUCKET_SIZE> entries_;
};
static_assert(sizeof(Bucket) == CACHE_LINE_SIZE);
}
