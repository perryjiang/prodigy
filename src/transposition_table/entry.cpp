#include "transposition_table/entry.h"

namespace prodigy::transposition_table {
Entry::Entry(const Key key, const Value value) : key_xor_value_(key ^ value.underlying()), value_(value) {}

std::pair<Key, Value> Entry::load() const {
  const auto value = value_.load(std::memory_order_relaxed);
  return {key_xor_value_.load(std::memory_order_relaxed) ^ value.underlying(), value};
}

void Entry::store(const Key key, const Value value) {
  key_xor_value_.store(key ^ value.underlying(), std::memory_order_relaxed);
  value_.store(value, std::memory_order_relaxed);
}
}
