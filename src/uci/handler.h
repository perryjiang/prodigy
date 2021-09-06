#pragma once

#include <iosfwd>
#include <memory>
#include <optional>
#include <string_view>

#include "board/position.h"
#include "search/controller.h"
#include "search/searcher.h"
#include "uci/messages.h"

namespace prodigy::uci {
class Handler final {
 public:
  Handler(std::ostream&, std::unique_ptr<search::Searcher>);

  Handler(const Handler&) = delete;
  Handler& operator=(const Handler&) = delete;

  bool handle(std::string_view);

 private:
  void handle(Uci);
  void handle(Debug) const;
  void handle(IsReady);
  void handle(SetOption&&) const;
  void handle(UciNewGame);
  void handle(Position&&);
  void handle(Go&&);
  void handle(Stop);
  void handle(PonderHit) const;
  void handle(Quit);

  template <typename... Args>
  void output(Args&&...);

  std::ostream& output_;
  std::optional<board::Position> position_;
  search::Controller controller_;
};
}
