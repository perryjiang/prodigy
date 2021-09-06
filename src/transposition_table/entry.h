#pragma once

#include <atomic>
#include <utility>

#include "transposition_table/key.h"
#include "transposition_table/value.h"

namespace prodigy::transposition_table {
class Entry final {
 public:
  Entry(Key, Value);

  std::pair<Key, Value> load() const;
  void store(Key, Value);

 private:
  static_assert(sizeof(Key) == sizeof(Value));
  std::atomic<Key> key_xor_value_;
  std::atomic<Value> value_;
};
}
