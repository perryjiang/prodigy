#define BOOST_TEST_MODULE Direction

#include "board/direction.h"

#include <boost/test/unit_test.hpp>
#include <optional>
#include <ostream>
#include <utility>

#include "board/coordinate.h"
#include "board/file.h"
#include "board/rank.h"

namespace prodigy::board {
std::ostream& boost_test_print_type(std::ostream& os, const std::optional<Coordinate> coordinate) {
  if (coordinate.has_value()) {
    os << *coordinate;
  } else {
    os << "nullopt";
  }
  return os;
}

namespace {
static_assert(sizeof(Direction) == 1);

BOOST_AUTO_TEST_CASE(test_directional_offset) {
  const auto north_rank = [](const auto coordinate) {
    return to_rank('1' + std::to_underlying(rank_of(coordinate)) + 1);
  };
  const auto east_file = [](const auto coordinate) {
    return to_file('a' + std::to_underlying(file_of(coordinate)) + 1);
  };
  const auto south_rank = [](const auto coordinate) {
    return to_rank('1' + std::to_underlying(rank_of(coordinate)) - 1);
  };
  const auto west_file = [](const auto coordinate) {
    return to_file('a' + std::to_underlying(file_of(coordinate)) - 1);
  };
  for_each_coordinate([&](const auto coordinate) {
    if (const auto north_offset = directional_offset<Direction::NORTH>(coordinate);
        rank_of(coordinate) == Rank::EIGHT) {
      BOOST_TEST(!north_offset.has_value());
    } else {
      BOOST_TEST(north_offset == to_coordinate(file_of(coordinate), north_rank(coordinate)));
    }
    if (const auto east_offset = directional_offset<Direction::EAST>(coordinate); file_of(coordinate) == File::H) {
      BOOST_TEST(!east_offset.has_value());
    } else {
      BOOST_TEST(east_offset == to_coordinate(east_file(coordinate), rank_of(coordinate)));
    }
    if (const auto south_offset = directional_offset<Direction::SOUTH>(coordinate); rank_of(coordinate) == Rank::ONE) {
      BOOST_TEST(!south_offset.has_value());
    } else {
      BOOST_TEST(south_offset == to_coordinate(file_of(coordinate), south_rank(coordinate)));
    }
    if (const auto west_offset = directional_offset<Direction::WEST>(coordinate); file_of(coordinate) == File::A) {
      BOOST_TEST(!west_offset.has_value());
    } else {
      BOOST_TEST(west_offset == to_coordinate(west_file(coordinate), rank_of(coordinate)));
    }
    if (const auto north_east_offset = directional_offset<Direction::NORTH_EAST>(coordinate);
        rank_of(coordinate) == Rank::EIGHT || file_of(coordinate) == File::H) {
      BOOST_TEST(!north_east_offset.has_value());
    } else {
      BOOST_TEST(north_east_offset == to_coordinate(east_file(coordinate), north_rank(coordinate)));
    }
    if (const auto south_east_offset = directional_offset<Direction::SOUTH_EAST>(coordinate);
        rank_of(coordinate) == Rank::ONE || file_of(coordinate) == File::H) {
      BOOST_TEST(!south_east_offset.has_value());
    } else {
      BOOST_TEST(south_east_offset == to_coordinate(east_file(coordinate), south_rank(coordinate)));
    }
    if (const auto south_west_offset = directional_offset<Direction::SOUTH_WEST>(coordinate);
        rank_of(coordinate) == Rank::ONE || file_of(coordinate) == File::A) {
      BOOST_TEST(!south_west_offset.has_value());
    } else {
      BOOST_TEST(south_west_offset == to_coordinate(west_file(coordinate), south_rank(coordinate)));
    }
    if (const auto north_west_offset = directional_offset<Direction::NORTH_WEST>(coordinate);
        rank_of(coordinate) == Rank::EIGHT || file_of(coordinate) == File::A) {
      BOOST_TEST(!north_west_offset.has_value());
    } else {
      BOOST_TEST(north_west_offset == to_coordinate(west_file(coordinate), north_rank(coordinate)));
    }
  });
}

BOOST_AUTO_TEST_CASE(test_unsafe_directional_offset) {
  for_each_coordinate([&](const auto coordinate) {
    if (const auto north_offset = directional_offset<Direction::NORTH>(coordinate); north_offset.has_value()) {
      BOOST_TEST(north_offset == unsafe_directional_offset<Direction::NORTH>(coordinate));
    }
    if (const auto east_offset = directional_offset<Direction::EAST>(coordinate); east_offset.has_value()) {
      BOOST_TEST(east_offset == unsafe_directional_offset<Direction::EAST>(coordinate));
    }
    if (const auto south_offset = directional_offset<Direction::SOUTH>(coordinate); south_offset.has_value()) {
      BOOST_TEST(south_offset == unsafe_directional_offset<Direction::SOUTH>(coordinate));
    }
    if (const auto west_offset = directional_offset<Direction::WEST>(coordinate); west_offset.has_value()) {
      BOOST_TEST(west_offset == unsafe_directional_offset<Direction::WEST>(coordinate));
    }
    if (const auto north_east_offset = directional_offset<Direction::NORTH_EAST>(coordinate);
        north_east_offset.has_value()) {
      BOOST_TEST(north_east_offset == unsafe_directional_offset<Direction::NORTH_EAST>(coordinate));
    }
    if (const auto south_east_offset = directional_offset<Direction::SOUTH_EAST>(coordinate);
        south_east_offset.has_value()) {
      BOOST_TEST(south_east_offset == unsafe_directional_offset<Direction::SOUTH_EAST>(coordinate));
    }
    if (const auto south_west_offset = directional_offset<Direction::SOUTH_WEST>(coordinate);
        south_west_offset.has_value()) {
      BOOST_TEST(south_west_offset == unsafe_directional_offset<Direction::SOUTH_WEST>(coordinate));
    }
    if (const auto north_west_offset = directional_offset<Direction::NORTH_WEST>(coordinate);
        north_west_offset.has_value()) {
      BOOST_TEST(north_west_offset == unsafe_directional_offset<Direction::NORTH_WEST>(coordinate));
    }
  });
}
}
}
