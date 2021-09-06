#pragma once

#include <functional>
#include <future>
#include <memory>
#include <optional>

#include "board/move.h"
#include "board/position.h"
#include "search/searcher.h"

namespace prodigy::search {
class Controller final {
 public:
  explicit Controller(std::unique_ptr<Searcher>);

  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;

  bool start_searching(board::Position&&, std::function<void(std::optional<board::Move>)>&&);
  bool stop_searching();

 private:
  const std::unique_ptr<Searcher> searcher_;
  std::future<void> search_;
};
}
