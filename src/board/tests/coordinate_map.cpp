#define BOOST_TEST_MODULE CoordinateMap

#include "board/coordinate_map.h"

#include <boost/test/unit_test.hpp>

#include "board/coordinate.h"

namespace prodigy::board {
namespace {
BOOST_AUTO_TEST_CASE(subscript) {
  CoordinateMap<Coordinate> coordinate_map;
  for_each_coordinate([&](const auto coordinate) { coordinate_map[coordinate] = coordinate; });
  for_each_coordinate([&](const auto coordinate) { BOOST_TEST(coordinate_map[coordinate] == coordinate); });
}

BOOST_AUTO_TEST_CASE(fill) {
  const auto construct_default = CoordinateMap<Coordinate>::fill();
  const auto construct_with_h8 = CoordinateMap<Coordinate>::fill(Coordinate::H8);
  for_each_coordinate([&](const auto coordinate) {
    BOOST_TEST(construct_default[coordinate] == Coordinate());
    BOOST_TEST(construct_with_h8[coordinate] == Coordinate::H8);
  });
}
}
}
