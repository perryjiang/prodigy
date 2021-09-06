#pragma once

#include <atomic>
#include <cstdint>
#include <optional>

#include "board/move.h"
#include "board/position.h"

namespace prodigy::search {
class Searcher {
 public:
  virtual ~Searcher() = default;

  Searcher(const Searcher&) = delete;
  Searcher& operator=(const Searcher&) = delete;

 protected:
  Searcher() = default;

  bool keep_searching() const { return state_.load(std::memory_order_relaxed) == State::SEARCHING; }

 private:
  friend class Controller;

  enum class State : std::uint8_t {
    IDLE,
    SEARCHING,
    STOPPING,
  };

  virtual std::optional<board::Move> search(const board::Position&) = 0;

  std::atomic<State> state_ = State::IDLE;
};
}
