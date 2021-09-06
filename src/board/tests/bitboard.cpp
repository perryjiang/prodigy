#define BOOST_TEST_MODULE Bitboard

#include "board/bitboard.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <queue>
#include <type_traits>

#include "board/coordinate.h"

namespace prodigy::board {
namespace {
static_assert(sizeof(Bitboard) == sizeof(std::uint64_t));
static_assert(std::is_trivially_copyable_v<Bitboard>);

static_assert(Bitboard().popcount() == 0);
static_assert(Bitboard(Coordinate::E1).popcount() == 1);
static_assert((Bitboard(Coordinate::E2) | Bitboard(Coordinate::E3)).popcount() == 2);
static_assert((~Bitboard()).popcount() == 64);

BOOST_AUTO_TEST_CASE(lsb) {
  for_each_coordinate([bitboard = ~Bitboard()](const auto coordinate) mutable {
    BOOST_TEST(bitboard.lsb() == Bitboard(coordinate));
    bitboard.pop_lsb();
  });
}

BOOST_AUTO_TEST_CASE(test_unsafe_to_coordinate) {
  for_each_coordinate(
      [](const auto coordinate) { BOOST_TEST(unsafe_to_coordinate(Bitboard(coordinate)) == coordinate); });
}

BOOST_AUTO_TEST_CASE(test_for_each_bit) {
  for_each_bit(Bitboard(), [](const auto bitboard) { BOOST_ERROR(bitboard); });

  std::queue<Bitboard> bitboards;
  for_each_coordinate([&](const auto coordinate) { bitboards.emplace(coordinate); });
  for_each_bit(~Bitboard(), [&](const auto bitboard) {
    BOOST_TEST_REQUIRE(!bitboards.empty());
    BOOST_TEST(bitboard == bitboards.front());
    bitboards.pop();
  });
  BOOST_TEST(bitboards.empty());
}

BOOST_AUTO_TEST_CASE(test_for_each_coordinate) {
  for_each_coordinate(Bitboard(), [](const auto coordinate) { BOOST_ERROR(coordinate); });

  std::queue<Coordinate> coordinates;
  for_each_coordinate([&](const auto coordinate) { coordinates.push(coordinate); });
  for_each_coordinate(~Bitboard(), [&](const auto coordinate) {
    BOOST_TEST_REQUIRE(!coordinates.empty());
    BOOST_TEST(coordinate == coordinates.front());
    coordinates.pop();
  });
  BOOST_TEST(coordinates.empty());
}

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << Bitboard();
  BOOST_TEST(os.is_equal("0x0000000000000000"));
  os << ~Bitboard();
  BOOST_TEST(os.is_equal("0xffffffffffffffff"));
  os << Bitboard(Coordinate::A1);
  BOOST_TEST(os.is_equal("0x0000000000000001"));
  os << Bitboard(Coordinate::H8);
  BOOST_TEST(os.is_equal("0x8000000000000000"));
  os << (Bitboard(Coordinate::B1) | Bitboard(Coordinate::B2) | Bitboard(Coordinate::B3));
  BOOST_TEST(os.is_equal("0x0000000000020202"));
}
}
}
