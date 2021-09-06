#pragma once

#include <array>
#include <boost/assert.hpp>
#include <charconv>
#include <concepts>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace prodigy {
template <std::invocable<std::string_view> Callback>
constexpr void split(const std::string_view view, Callback&& callback, const std::string_view delims = " ") {
  for (auto begin = 0UZ, end = 0UZ; begin < view.size() && end != std::string_view::npos; begin = end + 1) {
    if (end = view.find_first_of(delims, begin); begin != end) {
      std::forward<Callback>(callback)(view.substr(begin, end - begin));
    }
  }
}

template <std::size_t N>
constexpr std::array<std::string_view, N> split(const std::string_view view, const std::string_view delims = " ") {
  std::array<std::string_view, N> tokens;
  split(
      view,
      [&, it = tokens.begin()](const auto token) mutable {
        BOOST_ASSERT(it != tokens.end());
        *it++ = token;
      },
      delims);
  return tokens;
}

std::vector<std::string_view> split(std::string_view, std::string_view delims = " ");

template <typename T>
requires std::integral<T> || std::floating_point<T>
auto to_arithmetic(const std::string_view view) {
  std::remove_const_t<T> arithmetic;
  const auto [ptr, ec] = std::from_chars(view.begin(), view.end(), arithmetic);
  return ptr == view.end() && ec == std::errc() ? std::optional(arithmetic) : std::nullopt;
}

template <typename T>
requires std::integral<T> || std::floating_point<T>
auto unsafe_to_arithmetic(const std::string_view view) {
  const auto arithmetic = to_arithmetic<T>(view);
  BOOST_ASSERT(arithmetic.has_value());
  return *arithmetic;
};
}
