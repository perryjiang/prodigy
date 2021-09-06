#pragma once

#include <concepts>
#include <limits>
#include <random>

namespace prodigy {
template <std::integral T>
T uniform_distribution(const T a = 0, const T b = std::numeric_limits<T>::max()) {
  static std::default_random_engine engine(std::random_device{}());
  static std::uniform_int_distribution<T> uniform_distribution;
  return uniform_distribution(engine, typename decltype(uniform_distribution)::param_type(a, b));
}
}
