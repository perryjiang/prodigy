#include "uci/messages.h"

#include <boost/assert.hpp>
#include <span>
#include <unordered_set>

#include "base/string_utils.h"

namespace prodigy::uci {
IncomingMessage parse(const std::string_view message) {
  const auto tokens = split(message, " \f\n\r\t\v");
  for (std::span unparsed_tokens(tokens.data(), tokens.size()); !unparsed_tokens.empty();) {
    const auto pop_token = [&] {
      BOOST_ASSERT(!unparsed_tokens.empty());
      const auto token = unparsed_tokens.front();
      unparsed_tokens = unparsed_tokens.subspan<1>();
      return token;
    };
    const auto substr_until = [&](const auto end_token) {
      const auto first_token = pop_token();
      auto count = first_token.size();
      if (!unparsed_tokens.empty()) {
        for (auto token = pop_token(); token != end_token; token = pop_token()) {
          count = static_cast<std::size_t>(token.data() + token.size() - first_token.data());
          if (unparsed_tokens.empty()) {
            break;
          }
        }
      }
      return message.substr(static_cast<std::size_t>(first_token.data() - message.data()), count);
    };

    const auto command = pop_token();
    if (command == "uci") {
      return Uci();
    }
    if (command == "debug") {
      return Debug{
          .on = pop_token() == "on",
      };
    }
    if (command == "isready") {
      return IsReady();
    }
    if (command == "setoption") {
      BOOST_VERIFY(pop_token() == "name");
      SetOption set_option;
      set_option.name.assign(substr_until("value"));
      if (!unparsed_tokens.empty()) {
        set_option.value.emplace(message.substr(static_cast<std::size_t>(pop_token().data() - message.data())));
      }
      return set_option;
    }
    if (command == "ucinewgame") {
      return UciNewGame();
    }
    if (command == "position") {
      return Position{
          .position =
              [&] {
                if (pop_token() == "startpos") {
                  if (!unparsed_tokens.empty()) {
                    BOOST_VERIFY(pop_token() == "moves");
                  }
                  return board::Position::starting_position();
                }
                return board::Position::from_fen(substr_until("moves"));
              }(),
          .moves =
              [&] {
                std::vector<board::Move> moves;
                moves.reserve(unparsed_tokens.size());
                for (const auto move : unparsed_tokens) {
                  moves.emplace_back(move);
                }
                return moves;
              }(),
      };
    }
    if (command == "go") {
      Go go;
      while (!unparsed_tokens.empty()) {
        static const std::unordered_set<std::string_view> ARGS = {
            "searchmoves", "ponder", "wtime", "btime", "winc",     "binc",
            "movestogo",   "depth",  "nodes", "mate",  "movetime", "infinite",
        };
        const auto arg = pop_token();
        BOOST_ASSERT(ARGS.contains(arg));
        const auto pop_ms = [&] {
          return std::chrono::milliseconds(unsafe_to_arithmetic<std::chrono::milliseconds::rep>(pop_token()));
        };
        if (arg == "searchmoves") {
          for (auto move = pop_token();; move = pop_token()) {
            go.searchmoves.emplace_back(move);
            if (unparsed_tokens.empty() || ARGS.contains(unparsed_tokens.front())) {
              break;
            }
          }
        } else if (arg == "ponder") {
          go.ponder = true;
        } else if (arg == "wtime") {
          go.wtime = pop_ms();
        } else if (arg == "btime") {
          go.btime = pop_ms();
        } else if (arg == "winc") {
          go.winc = pop_ms();
        } else if (arg == "binc") {
          go.binc = pop_ms();
        } else if (arg == "movestogo") {
          go.movestogo = unsafe_to_arithmetic<decltype(go.movestogo)::value_type>(pop_token());
        } else if (arg == "depth") {
          go.depth = to_arithmetic<decltype(go.depth)::value_type>(pop_token());
        } else if (arg == "nodes") {
          go.nodes = unsafe_to_arithmetic<decltype(go.nodes)::value_type>(pop_token());
        } else if (arg == "mate") {
          go.mate = unsafe_to_arithmetic<decltype(go.mate)::value_type>(pop_token());
        } else if (arg == "movetime") {
          go.movetime = pop_ms();
        } else if (arg == "infinite") {
          go.infinite = true;
        }
      }
      return go;
    }
    if (command == "stop") {
      return Stop();
    }
    if (command == "ponderhit") {
      return PonderHit();
    }
    if (command == "quit") {
      return Quit();
    }
  }
  return {};
}
}
