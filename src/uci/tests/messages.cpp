#define BOOST_TEST_MODULE Messages

#include "uci/messages.h"

#include <boost/test/unit_test.hpp>
#include <chrono>
#include <functional>
#include <optional>
#include <string_view>
#include <type_traits>
#include <variant>

#include "board/coordinate.h"
#include "board/move.h"
#include "board/starting_position_fen.h"

namespace prodigy::uci {
namespace {
template <typename T>
void expect(
    const std::string_view incoming_message, const std::function<void(const T&)>& callback = [](auto&&) {}) {
  const auto parsed_message = parse(incoming_message);
  BOOST_TEST(std::holds_alternative<T>(parsed_message));
  callback(std::get<T>(parsed_message));
}

BOOST_AUTO_TEST_CASE(parse_uci) { expect<Uci>("uci"); }

BOOST_AUTO_TEST_CASE(parse_debug) {
  expect<Debug>("debug on", [](const auto debug) { BOOST_TEST(debug.on); });
  expect<Debug>("debug off", [](const auto debug) { BOOST_TEST(!debug.on); });
}

BOOST_AUTO_TEST_CASE(parse_isready) { expect<IsReady>("isready"); }

BOOST_AUTO_TEST_CASE(parse_setoption) {
  const auto expect = [](const auto message, const auto name,
                         const std::optional<std::string_view> value = std::nullopt) {
    uci::expect<SetOption>(message, [&](const auto& set_option) {
      BOOST_TEST(set_option.name == name);
      BOOST_TEST(set_option.value.has_value() == value.has_value());
      if (set_option.value.has_value() && value.has_value()) {
        BOOST_TEST(*set_option.value == *value);
      }
    });
  };

  expect("setoption name foo value bar", "foo", "bar");
  expect("setoption name foo bar value baz", "foo bar", "baz");
  expect("setoption name foo value bar baz", "foo", "bar baz");
  expect("setoption name hello   world value foo bar \tbaz", "hello   world", "foo bar \tbaz");

  expect("setoption name foo", "foo");
  expect("setoption name foo\tbar", "foo\tbar");
}

BOOST_AUTO_TEST_CASE(parse_ucinewgame) { expect<UciNewGame>("ucinewgame"); }

BOOST_AUTO_TEST_CASE(parse_position) {
  expect<Position>("position startpos", [](const auto& position) {
    BOOST_TEST(position.position.fen() == board::STARTING_POSITION_FEN);
    BOOST_TEST(position.moves.empty());
  });
  expect<Position>("position startpos moves e2e4", [](const auto& position) {
    BOOST_TEST(position.position.fen() == board::STARTING_POSITION_FEN);
    BOOST_TEST_REQUIRE(position.moves.size() == 1);
    BOOST_TEST(position.moves[0] == board::Move(board::Coordinate::E2, board::Coordinate::E4));
  });
  expect<Position>("position startpos moves e2e4 e7e5", [](const auto& position) {
    BOOST_TEST(position.position.fen() == board::STARTING_POSITION_FEN);
    BOOST_TEST_REQUIRE(position.moves.size() == 2);
    BOOST_TEST(position.moves[0] == board::Move(board::Coordinate::E2, board::Coordinate::E4));
    BOOST_TEST(position.moves[1] == board::Move(board::Coordinate::E7, board::Coordinate::E5));
  });
  expect<Position>(
      "position fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1", [](const auto& position) {
        BOOST_TEST(position.position.fen() == "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1");
        BOOST_TEST(position.moves.empty());
      });
  expect<Position>(
      "position fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1 moves a8b8",
      [](const auto& position) {
        BOOST_TEST(position.position.fen() == "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1");
        BOOST_TEST_REQUIRE(position.moves.size() == 1);
        BOOST_TEST(position.moves[0] == board::Move(board::Coordinate::A8, board::Coordinate::B8));
      });
  expect<Position>(
      "position fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1 moves a8b8 a7a8q",
      [](const auto& position) {
        BOOST_TEST(position.position.fen() == "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1");
        BOOST_TEST_REQUIRE(position.moves.size() == 2);
        BOOST_TEST(position.moves[0] == board::Move(board::Coordinate::A8, board::Coordinate::B8));
        BOOST_TEST(position.moves[1] ==
                   board::Move(board::Coordinate::A7, board::Coordinate::A8, board::PieceType::QUEEN));
      });
}

BOOST_AUTO_TEST_CASE(parse_go) {
  for (const auto message : {"go", "go depth 256"}) {
    expect<Go>(message, [](const auto& go) {
      BOOST_TEST(go.searchmoves.empty());
      BOOST_TEST(!go.ponder);
      BOOST_TEST(!go.wtime.has_value());
      BOOST_TEST(!go.btime.has_value());
      BOOST_TEST(!go.winc.has_value());
      BOOST_TEST(!go.binc.has_value());
      BOOST_TEST(!go.movestogo.has_value());
      BOOST_TEST(!go.depth.has_value());
      BOOST_TEST(!go.nodes.has_value());
      BOOST_TEST(!go.mate.has_value());
      BOOST_TEST(!go.movetime.has_value());
      BOOST_TEST(!go.infinite);
    });
  }
  expect<Go>("go searchmoves e2e4 ponder wtime 1 btime 2 winc 3 binc 4 movestogo 5 depth 6 nodes 7 mate 8 movetime 9",
             [&](const auto& go) {
               using namespace std::chrono_literals;
               const auto expect_opt = [](const auto opt, const auto expected) {
                 BOOST_TEST_REQUIRE(opt.has_value());
                 if constexpr (std::is_same_v<std::remove_const_t<decltype(expected)>, std::chrono::milliseconds>) {
                   BOOST_TEST(opt->count() == expected.count());
                 } else {
                   BOOST_TEST(*opt == expected);
                 }
               };
               BOOST_TEST_REQUIRE(go.searchmoves.size() == 1);
               BOOST_TEST(go.searchmoves[0] == board::Move(board::Coordinate::E2, board::Coordinate::E4));
               BOOST_TEST(go.ponder);
               expect_opt(go.wtime, 1ms);
               expect_opt(go.btime, 2ms);
               expect_opt(go.winc, 3ms);
               expect_opt(go.binc, 4ms);
               expect_opt(go.movestogo, 5UZ);
               expect_opt(go.depth, 6UZ);
               expect_opt(go.nodes, 7UZ);
               expect_opt(go.mate, 8UZ);
               expect_opt(go.movetime, 9ms);
               BOOST_TEST(!go.infinite);
             });
  expect<Go>("go infinite searchmoves e2e4 e7e5", [&](const auto& go) {
    BOOST_TEST_REQUIRE(go.searchmoves.size() == 2);
    BOOST_TEST(go.searchmoves[0] == board::Move(board::Coordinate::E2, board::Coordinate::E4));
    BOOST_TEST(go.searchmoves[1] == board::Move(board::Coordinate::E7, board::Coordinate::E5));
    BOOST_TEST(!go.ponder);
    BOOST_TEST(!go.wtime.has_value());
    BOOST_TEST(!go.btime.has_value());
    BOOST_TEST(!go.winc.has_value());
    BOOST_TEST(!go.binc.has_value());
    BOOST_TEST(!go.movestogo.has_value());
    BOOST_TEST(!go.depth.has_value());
    BOOST_TEST(!go.nodes.has_value());
    BOOST_TEST(!go.mate.has_value());
    BOOST_TEST(!go.movetime.has_value());
    BOOST_TEST(go.infinite);
  });
}

BOOST_AUTO_TEST_CASE(parse_stop) { expect<Stop>("stop"); }

BOOST_AUTO_TEST_CASE(parse_ponderhit) { expect<PonderHit>("ponderhit"); }

BOOST_AUTO_TEST_CASE(parse_quit) { expect<Quit>("quit"); }

BOOST_AUTO_TEST_CASE(parse_arbitrary_whitespace) {
  expect<Position>("  position       startpos \r moves e2e4 \t\t\te7e5", [](const auto& position) {
    BOOST_TEST(position.position.fen() == board::STARTING_POSITION_FEN);
    BOOST_TEST_REQUIRE(position.moves.size() == 2);
    BOOST_TEST(position.moves[0] == board::Move(board::Coordinate::E2, board::Coordinate::E4));
    BOOST_TEST(position.moves[1] == board::Move(board::Coordinate::E7, board::Coordinate::E5));
  });
}

BOOST_AUTO_TEST_CASE(parse_unknown_token) {
  expect<Uci>("unknown uci");
  expect<Debug>("unknown debug on", [](const auto debug) { BOOST_TEST(debug.on); });
  expect<Quit>("unknown1 unknown2 quit");
}
}
}
