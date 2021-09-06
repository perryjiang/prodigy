#define BOOST_TEST_MODULE PieceType

#include "board/piece_type.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

namespace prodigy::board {
namespace {
static_assert(sizeof(PieceType) == 1);

static_assert(to_piece_type('p') == PieceType::PAWN);
static_assert(to_piece_type('n') == PieceType::KNIGHT);
static_assert(to_piece_type('b') == PieceType::BISHOP);
static_assert(to_piece_type('r') == PieceType::ROOK);
static_assert(to_piece_type('q') == PieceType::QUEEN);
static_assert(to_piece_type('k') == PieceType::KING);

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << PieceType::PAWN;
  BOOST_TEST(os.is_equal("p"));
  os << PieceType::KNIGHT;
  BOOST_TEST(os.is_equal("n"));
  os << PieceType::BISHOP;
  BOOST_TEST(os.is_equal("b"));
  os << PieceType::ROOK;
  BOOST_TEST(os.is_equal("r"));
  os << PieceType::QUEEN;
  BOOST_TEST(os.is_equal("q"));
  os << PieceType::KING;
  BOOST_TEST(os.is_equal("k"));
}
}
}
