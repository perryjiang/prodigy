#include "search/controller.h"

#include <boost/assert.hpp>
#include <utility>

namespace prodigy::search {
Controller::Controller(std::unique_ptr<Searcher> searcher) : searcher_(std::move(searcher)) { BOOST_ASSERT(searcher_); }

bool Controller::start_searching(board::Position&& position,
                                 std::function<void(std::optional<board::Move>)>&& callback) {
  if (auto expected = Searcher::State::IDLE;
      !searcher_->state_.compare_exchange_strong(expected, Searcher::State::SEARCHING, std::memory_order_relaxed)) {
    return false;
  }
  search_ = std::async(std::launch::async, [&, position = std::move(position), callback = std::move(callback)] {
    const auto move = searcher_->search(position);
    searcher_->state_.store(Searcher::State::IDLE, std::memory_order_relaxed);
    callback(move);
  });
  return true;
}

bool Controller::stop_searching() {
  auto expected = Searcher::State::SEARCHING;
  return searcher_->state_.compare_exchange_strong(expected, Searcher::State::STOPPING, std::memory_order_relaxed);
}
}
