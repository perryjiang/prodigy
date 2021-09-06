#include "uci/handler.h"

#include <iostream>
#include <syncstream>
#include <utility>
#include <variant>

#include "board/color.h"

namespace prodigy::uci {
namespace {
template <typename... Ts>
struct visitor : public Ts... {
  using Ts::operator()...;
};
}

Handler::Handler(std::ostream& output, std::unique_ptr<search::Searcher> searcher)
    : output_(output), controller_(std::move(searcher)) {}

bool Handler::handle(const std::string_view view) {
  auto message = parse(view);
  std::visit(visitor{
                 [&](auto&& message) { handle(std::move(message)); },
                 [&](std::monostate) { std::clog << __func__ << ": " << view << '\n'; },
             },
             std::move(message));
  return !std::holds_alternative<Quit>(message);
}

void Handler::handle(Uci) { output("id name Prodigy\n", "id author Perry Jiang\n", "uciok"); }

void Handler::handle(Debug) const {}

void Handler::handle(IsReady) { output("readyok"); }

void Handler::handle(SetOption&&) const {}

void Handler::handle(UciNewGame) { position_.reset(); }

void Handler::handle(Position&& position) {
  position_ = std::move(position.position);
  for (const auto move : position.moves) {
    position_ = position_->active_color() == board::Color::WHITE ? position_->apply<board::Color::WHITE>(move)
                                                                 : position_->apply<board::Color::BLACK>(move);
  }
}

void Handler::handle(Go&&) {
  if (!position_.has_value()) {
    std::clog << __func__ << ": unknown position\n";
    return;
  }
  controller_.start_searching(std::move(*position_), [&](const auto move) {
    move.has_value() ? output("bestmove ", *move) : output("bestmove 0000");
  });
  position_.reset();
}

void Handler::handle(Stop) { controller_.stop_searching(); }

void Handler::handle(PonderHit) const {}

void Handler::handle(Quit) { controller_.stop_searching(); }

template <typename... Args>
void Handler::output(Args&&... args) {
  (std::osyncstream(output_) << ... << std::forward<Args>(args)) << std::endl;
}
}
