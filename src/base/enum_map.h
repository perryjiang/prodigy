#pragma once

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "base/fill_array.h"

namespace prodigy {
template <typename E, typename T, std::size_t N>
requires std::is_enum_v<E>
class EnumMap final : private std::array<T, N> {
  using Base = std::array<T, N>;

 public:
  using Base::Base;

  template <typename... Args>
  static constexpr EnumMap fill(const Args&... args) {
    return EnumMap(nullptr, args...);
  }

  constexpr Base::reference operator[](const E e) { return (*this)[std::to_underlying(e)]; }
  constexpr Base::const_reference operator[](const E e) const { return (*this)[std::to_underlying(e)]; }

  using Base::begin;
  using Base::end;

 private:
  template <typename... Args>
  EnumMap(std::nullptr_t, const Args&... args) : Base(fill_array<T, N>(args...)) {}

  using Base::operator[];
};
}
