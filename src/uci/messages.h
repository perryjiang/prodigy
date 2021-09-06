#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "base/ply.h"
#include "board/move.h"
#include "board/position.h"

namespace prodigy::uci {
struct Uci final {};

struct Debug final {
  bool on;
};

struct IsReady final {};

struct SetOption final {
  std::string name;
  std::optional<std::string> value;
};

struct UciNewGame final {};

struct Position final {
  board::Position position;
  std::vector<board::Move> moves;
};

struct Go final {
  std::vector<board::Move> searchmoves;
  bool ponder = false;
  std::optional<std::chrono::milliseconds> wtime;
  std::optional<std::chrono::milliseconds> btime;
  std::optional<std::chrono::milliseconds> winc;
  std::optional<std::chrono::milliseconds> binc;
  std::optional<std::size_t> movestogo;
  std::optional<Ply> depth;
  std::optional<std::size_t> nodes;
  std::optional<std::size_t> mate;
  std::optional<std::chrono::milliseconds> movetime;
  bool infinite = false;
};

struct Stop final {};

struct PonderHit final {};

struct Quit final {};

using IncomingMessage =
    std::variant<std::monostate, Uci, Debug, IsReady, SetOption, UciNewGame, Position, Go, Stop, PonderHit, Quit>;

IncomingMessage parse(std::string_view);
}
