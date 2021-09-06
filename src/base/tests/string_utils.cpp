#define BOOST_TEST_MODULE StringUtils

#include "base/string_utils.h"

#include <array>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace prodigy {
namespace {
void expect(const std::vector<std::string_view>& split, const std::vector<std::string_view>& expected) {
  BOOST_TEST_REQUIRE(split.size() == expected.size());
  for (auto i = 0UZ; i < split.size(); ++i) {
    BOOST_TEST(split[i] == expected[i]);
  }
}

template <std::size_t N>
void expect(const std::array<std::string_view, N>& split, const std::array<std::string_view, N>& expected) {
  expect({split.begin(), split.end()}, {expected.begin(), expected.end()});
}

template <typename T>
void expect(const std::string_view view, const std::optional<T> expected) {
  const auto result = to_arithmetic<T>(view);
  BOOST_TEST_REQUIRE(result.has_value() == expected.has_value());
  if (result.has_value()) {
    BOOST_TEST(*result == *expected);
    BOOST_TEST(*result == unsafe_to_arithmetic<T>(view));
  }
}

BOOST_AUTO_TEST_CASE(split_empty) { expect(split("", " @"), {}); }

BOOST_AUTO_TEST_CASE(split_all_delimiters) { expect(split("@!@!@!!!!@@@!!!", " @!"), {}); }

BOOST_AUTO_TEST_CASE(split_single_delimiter_single_token) { expect(split("      foo    "), {"foo"}); }

BOOST_AUTO_TEST_CASE(split_multi_delimiter_single_token) { expect(split("!    !foo  !  ", " !"), {"foo"}); }

BOOST_AUTO_TEST_CASE(split_single_delimiter_multi_token) { expect(split("foo bar  baz"), {"foo", "bar", "baz"}); }

BOOST_AUTO_TEST_CASE(split_multi_delimiter_multi_token) {
  expect(split("foo@bar!!baz@!@!foo", "@!"), {"foo", "bar", "baz", "foo"});
}

BOOST_AUTO_TEST_CASE(split_single_delimiter_trim) { expect(split("  foo bar  baz "), {"foo", "bar", "baz"}); }

BOOST_AUTO_TEST_CASE(split_multi_delimiter_trim) {
  expect(split("@@foo@bar!!baz@!@!foo!", "@!"), {"foo", "bar", "baz", "foo"});
}

BOOST_AUTO_TEST_CASE(split_duplicated_delimiters) {
  expect(split("@@ foo @bar @@baz", "@@"), {" foo ", "bar ", "baz"});
}

BOOST_AUTO_TEST_CASE(split_into_array) { expect(split<3>(" foo  bar   baz    "), {"foo", "bar", "baz"}); }

BOOST_AUTO_TEST_CASE(split_into_array_too_large) {
  expect(split<6>(" foo  bar   baz"), {"foo", "bar", "baz", "", "", ""});
}

BOOST_AUTO_TEST_CASE(to_arithmetic_signed) { expect<std::int8_t>("-100", -100); }

BOOST_AUTO_TEST_CASE(to_arithmetic_unsigned) { expect<std::uint8_t>("100", 100); }

BOOST_AUTO_TEST_CASE(to_arithmetic_limits) {
  static constexpr auto max = std::numeric_limits<std::int32_t>::max();
  expect<std::int32_t>(std::to_string(max), max);

  static constexpr auto min = std::numeric_limits<std::int32_t>::min();
  expect<std::int32_t>(std::to_string(min), min);
}

BOOST_AUTO_TEST_CASE(to_arithmetic_out_of_range) {
  expect<std::uint8_t>("-1", std::nullopt);
  expect<std::int8_t>("1000", std::nullopt);
}

BOOST_AUTO_TEST_CASE(to_arithmetic_invalid_input) {
  expect<std::uint8_t>("1foo", std::nullopt);
  expect<std::uint8_t>(" 1", std::nullopt);
  expect<std::int8_t>("bar-10", std::nullopt);
  expect<std::int8_t>("-10 ", std::nullopt);
}
}
}
