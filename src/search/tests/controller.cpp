#define BOOST_TEST_MODULE Controller

#include "search/controller.h"

#include <atomic>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <memory>
#include <optional>
#include <ostream>
#include <thread>

#include "board/coordinate.h"
#include "board/move.h"
#include "board/position.h"
#include "search/searcher.h"

namespace prodigy::board {
std::ostream& boost_test_print_type(std::ostream& os, const std::optional<Move> move) {
  if (move.has_value()) {
    os << *move;
  } else {
    os << "nullopt";
  }
  return os;
}
}

namespace prodigy::search {
namespace {
class NoopSearcher final : public Searcher {
 public:
  explicit NoopSearcher(const std::optional<board::Move> move = std::nullopt) : move_(move) {}

 private:
  std::optional<board::Move> search(const board::Position&) override {
    while (keep_searching()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return move_;
  }

  const std::optional<board::Move> move_;
};

BOOST_AUTO_TEST_CASE(start_searching_while_searching) {
  Controller controller(std::make_unique<NoopSearcher>());
  BOOST_TEST(controller.start_searching(board::Position(board::Position::starting_position()), [](auto&&) {}));
  BOOST_TEST(!controller.start_searching(board::Position(board::Position::starting_position()), [](auto&&) {}));
  BOOST_TEST(controller.stop_searching());
}

BOOST_AUTO_TEST_CASE(stop_searching_while_not_searching) {
  Controller controller(std::make_unique<NoopSearcher>());
  BOOST_TEST(!controller.stop_searching());
  BOOST_TEST(controller.start_searching(board::Position(board::Position::starting_position()), [](auto&&) {}));
  BOOST_TEST(controller.stop_searching());
  BOOST_TEST(!controller.stop_searching());
}

BOOST_AUTO_TEST_CASE(search) {
  const auto move = board::Move(board::Coordinate::H2, board::Coordinate::H3);
  Controller controller(std::make_unique<NoopSearcher>(move));
  const auto search_once = [&] {
    std::atomic<std::optional<board::Move>> search_result;
    BOOST_TEST(controller.start_searching(board::Position(board::Position::starting_position()),
                                          [&](const auto move) { search_result = move; }));
    BOOST_TEST(controller.stop_searching());
    while (!search_result.load().has_value()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    BOOST_TEST(search_result.load() == move);
  };
  search_once();
  search_once();
}
}
}
