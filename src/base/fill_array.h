#pragma once

#include <array>
#include <cstddef>
#include <utility>

namespace prodigy {
template <typename T, std::size_t N, typename... Args>
std::array<T, N> fill_array(const Args&... args) {
  return [&]<auto... Is>(std::index_sequence<Is...>)->std::array<T, N> {
    return {(static_cast<void>(Is), T(args...))...};
  }
  (std::make_index_sequence<N>());
}
}
