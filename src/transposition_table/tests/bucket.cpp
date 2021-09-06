#define BOOST_TEST_MODULE Bucket

#include "transposition_table/bucket.h"

#include <boost/test/unit_test.hpp>
#include <optional>
#include <ostream>

#include "base/ply.h"
#include "board/coordinate.h"
#include "board/move.h"
#include "search/node_type.h"
#include "transposition_table/key.h"
#include "transposition_table/value.h"

namespace prodigy::transposition_table {
std::ostream& boost_test_print_type(std::ostream& os, const std::optional<Value>& opt) {
  if (opt.has_value()) {
    os << *opt;
  } else {
    os << "nullopt";
  }
  return os;
}

namespace {
BOOST_AUTO_TEST_CASE(find) {
  static constexpr Key KEY = 0;
  static constexpr Ply BASE_GENERATION = 1;
  const auto make_value = [](const auto generation) {
    return Value(board::Move(board::Coordinate::E2, board::Coordinate::E4), 5, 3, search::NodeType::PV, generation);
  };

  Bucket bucket;
  BOOST_TEST(!Bucket().find(0, 0).has_value());
  BOOST_TEST(bucket.try_insert(KEY, make_value(BASE_GENERATION)));
  BOOST_TEST(bucket.find(KEY, BASE_GENERATION) == make_value(BASE_GENERATION));
  BOOST_TEST(bucket.find(KEY, BASE_GENERATION + 1) == make_value(BASE_GENERATION + 1));
}

BOOST_AUTO_TEST_CASE(try_insert) {
  static constexpr Key KEY = 0;
  static constexpr Ply GENERATION = 0;
  const auto make_value = [](const auto depth, const Ply generation = 0) {
    return Value(board::Move(board::Coordinate::E2, board::Coordinate::E4), 5, depth, search::NodeType::PV, generation);
  };

  Bucket bucket;
  BOOST_TEST(bucket.try_insert(KEY, make_value(5)));
  BOOST_TEST(bucket.find(KEY, GENERATION) == make_value(5));
  BOOST_TEST(bucket.try_insert(KEY, make_value(10)));
  BOOST_TEST(bucket.find(KEY, GENERATION) == make_value(10));
  BOOST_TEST(!bucket.try_insert(KEY, make_value(9, GENERATION + 1)));
  BOOST_TEST(bucket.try_insert(KEY, make_value(11, GENERATION + 1)));
}

BOOST_AUTO_TEST_CASE(evict_ancient_same_depth) {
  static constexpr Ply GENERATION = Bucket::BUCKET_SIZE;
  const auto make_value = [](const auto generation) {
    return Value(board::Move(board::Coordinate::E2, board::Coordinate::E4), 5, 3, search::NodeType::PV, generation);
  };

  Bucket bucket;
  for (auto key = 0UZ; key < Bucket::BUCKET_SIZE; ++key) {
    BOOST_TEST(bucket.try_insert(key, make_value(key)));
  }
  for (auto key = Bucket::BUCKET_SIZE; key < Bucket::BUCKET_SIZE * 2; ++key) {
    BOOST_TEST(bucket.try_insert(key, make_value(GENERATION)));
  }
  for (auto key = Bucket::BUCKET_SIZE; key < Bucket::BUCKET_SIZE * 2; ++key) {
    BOOST_TEST(bucket.find(key, GENERATION) == make_value(GENERATION));
  }
}

BOOST_AUTO_TEST_CASE(evict_smallest_depth_same_generation) {
  static constexpr Ply GENERATION = 0;
  const auto make_value = [](const auto depth) {
    return Value(board::Move(board::Coordinate::E2, board::Coordinate::E4), 5, depth, search::NodeType::PV, GENERATION);
  };

  Bucket bucket;
  for (auto key = 0UZ; key < Bucket::BUCKET_SIZE; ++key) {
    BOOST_TEST(bucket.try_insert(key, make_value(10 - key)));
  }
  BOOST_TEST(bucket.try_insert(Bucket::BUCKET_SIZE, make_value(Bucket::BUCKET_SIZE)));
  for (auto key = 0UZ; key < Bucket::BUCKET_SIZE; ++key) {
    if (key == Bucket::BUCKET_SIZE - 1) {
      BOOST_TEST(bucket.find(Bucket::BUCKET_SIZE, GENERATION) == make_value(Bucket::BUCKET_SIZE));
      BOOST_TEST(!bucket.find(Bucket::BUCKET_SIZE - 1, GENERATION).has_value());
    } else {
      BOOST_TEST(bucket.find(key, GENERATION) == make_value(10 - key));
    }
  }
}

BOOST_AUTO_TEST_CASE(evict_least_valuable) {
  static constexpr Ply BASE_DEPTH = 3;
  static constexpr Ply GENERATION = 0;
  const auto make_value = [](const auto depth, const auto generation) {
    return Value(board::Move(board::Coordinate::E2, board::Coordinate::E4), 5, depth, search::NodeType::PV, generation);
  };

  Bucket bucket;
  for (auto key = 0UZ; key < Bucket::BUCKET_SIZE; ++key) {
    BOOST_TEST(bucket.try_insert(key, make_value(BASE_DEPTH + key, GENERATION)));
  }
  BOOST_TEST(bucket.try_insert(Bucket::BUCKET_SIZE, make_value(BASE_DEPTH - 1, GENERATION + 1)));
  for (auto key = 0UZ; key < Bucket::BUCKET_SIZE; ++key) {
    if (key == 0UZ) {
      BOOST_TEST(bucket.find(Bucket::BUCKET_SIZE, GENERATION + 1) == make_value(BASE_DEPTH - 1, GENERATION + 1));
      BOOST_TEST(!bucket.find(0UZ, GENERATION).has_value());
    } else {
      BOOST_TEST(bucket.find(key, GENERATION) == make_value(BASE_DEPTH + key, GENERATION));
    }
  }
}
}
}
