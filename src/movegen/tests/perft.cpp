#define BOOST_TEST_MODULE Perft

#include "movegen/perft.h"

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

#include "base/ply.h"
#include "board/position.h"
#include "board/starting_position_fen.h"
#include "movegen/move_generator.h"

namespace prodigy::movegen {
namespace {
void expect(const std::string_view fen, const std::vector<std::uint64_t>& expected_depth_to_node_count) {
  static const MoveGenerator MOVE_GENERATOR;

#if !defined(NDEBUG) || defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__) || defined(PRODIGY_SANITIZE_UB)
  const auto depth = std::min(5UZ, expected_depth_to_node_count.size() - 1);
#else
  const auto depth = expected_depth_to_node_count.size() - 1;
#endif
  BOOST_TEST_REQUIRE(std::in_range<Ply>(depth));
  BOOST_TEST_REQUIRE(depth < expected_depth_to_node_count.size());

  const auto result = perft(MOVE_GENERATOR, board::Position::from_fen(fen), depth);
  BOOST_TEST_REQUIRE(result.depth_to_node_count.size() - 1 == depth);

  for (auto i = 0UZ; i < result.depth_to_node_count.size(); ++i) {
    BOOST_TEST_REQUIRE(result.depth_to_node_count[i] == expected_depth_to_node_count[i]);
  }
}

BOOST_AUTO_TEST_CASE(one) {
  expect(board::STARTING_POSITION_FEN, {
                                           1,
                                           20,
                                           400,
                                           8'902,
                                           197'281,
                                           4'865'609,
                                           119'060'324,
                                           3'195'901'860,
                                       });
}

BOOST_AUTO_TEST_CASE(two) {
  expect("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", {
                                                                                     1,
                                                                                     48,
                                                                                     2'039,
                                                                                     97'862,
                                                                                     4'085'603,
                                                                                     193'690'690,
                                                                                     8'031'647'685,
                                                                                 });
}

BOOST_AUTO_TEST_CASE(three) {
  expect("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", {
                                                          1,
                                                          14,
                                                          191,
                                                          2'812,
                                                          43'238,
                                                          674'624,
                                                          11'030'083,
                                                          178'633'661,
                                                          3'009'794'393,
                                                      });
}

BOOST_AUTO_TEST_CASE(four) {
  expect("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", {
                                                                                 1,
                                                                                 6,
                                                                                 264,
                                                                                 9'467,
                                                                                 422'333,
                                                                                 15'833'292,
                                                                                 706'045'033,
                                                                             });
}

BOOST_AUTO_TEST_CASE(five) {
  expect("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", {
                                                                          1,
                                                                          44,
                                                                          1'486,
                                                                          62'379,
                                                                          2'103'487,
                                                                          89'941'194,
                                                                      });
}

BOOST_AUTO_TEST_CASE(six) {
  expect("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", {
                                                                                         1,
                                                                                         46,
                                                                                         2'079,
                                                                                         89'890,
                                                                                         3'894'594,
                                                                                         164'075'551,
                                                                                         6'923'051'137,
                                                                                     });
}

BOOST_AUTO_TEST_CASE(seven) {
  expect("rnbQkbnr/3ppppp/p1p5/8/8/2P5/PP1PPPPP/RNB1KBNR b KQkq - 0 4", {
                                                                            1,
                                                                            1,
                                                                            19,
                                                                            342,
                                                                            7'095,
                                                                            140'931,
                                                                            3'151'343,
                                                                            67'820'026,
                                                                        });
}

BOOST_AUTO_TEST_CASE(eight) {
  expect("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", {
                                                        1,
                                                        24,
                                                        496,
                                                        9'483,
                                                        182'838,
                                                        3'605'103,
                                                        71'179'139,
                                                    });
}
}
}
