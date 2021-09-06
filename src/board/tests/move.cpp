#define BOOST_TEST_MODULE Move

#include "board/move.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <optional>
#include <ostream>
#include <sstream>
#include <type_traits>

#include "board/color.h"
#include "board/color_traits.h"
#include "board/coordinate.h"
#include "board/piece_type.h"

namespace prodigy::board {
std::ostream& boost_test_print_type(std::ostream& os, const std::optional<PieceType> piece_type) {
  if (piece_type.has_value()) {
    os << *piece_type;
  } else {
    os << "nullopt";
  }
  return os;
}

namespace {
static_assert(sizeof(Move) == 2);
static_assert(std::is_trivially_copyable_v<Move>);

constexpr Move MOVE(Coordinate::C1, Coordinate::C2);
static_assert(MOVE.origin() == Coordinate::C1);
static_assert(MOVE.target() == Coordinate::C2);
static_assert(!MOVE.promotion().has_value());

constexpr Move KNIGHT_PROMOTION(Coordinate::A7, Coordinate::A8, PieceType::KNIGHT);
static_assert(KNIGHT_PROMOTION.origin() == Coordinate::A7);
static_assert(KNIGHT_PROMOTION.target() == Coordinate::A8);
static_assert(KNIGHT_PROMOTION.promotion() == PieceType::KNIGHT);

constexpr Move BISHOP_PROMOTION(Coordinate::B7, Coordinate::B8, PieceType::BISHOP);
static_assert(BISHOP_PROMOTION.origin() == Coordinate::B7);
static_assert(BISHOP_PROMOTION.target() == Coordinate::B8);
static_assert(BISHOP_PROMOTION.promotion() == PieceType::BISHOP);

constexpr Move ROOK_PROMOTION(Coordinate::C7, Coordinate::C8, PieceType::ROOK);
static_assert(ROOK_PROMOTION.origin() == Coordinate::C7);
static_assert(ROOK_PROMOTION.target() == Coordinate::C8);
static_assert(ROOK_PROMOTION.promotion() == PieceType::ROOK);

constexpr Move QUEEN_PROMOTION(Coordinate::D7, Coordinate::D8, PieceType::QUEEN);
static_assert(QUEEN_PROMOTION.origin() == Coordinate::D7);
static_assert(QUEEN_PROMOTION.target() == Coordinate::D8);
static_assert(QUEEN_PROMOTION.promotion() == PieceType::QUEEN);

BOOST_AUTO_TEST_CASE(test_move_constructor) {
  for_each_coordinate([](const auto origin) {
    for_each_coordinate([&](const auto target) {
      const auto test_move = [&](const std::optional<PieceType> promotion = std::nullopt) {
        std::ostringstream oss;
        oss << origin << target;
        if (promotion.has_value()) {
          oss << *promotion;
        }
        const Move move(oss.view());
        BOOST_TEST(move.origin() == origin);
        BOOST_TEST(move.target() == target);
        BOOST_TEST(move.promotion() == promotion);
        BOOST_TEST(move == Move(origin, target, promotion));
      };
      const auto maybe_test_promotion = [&]<Color ACTIVE_COLOR> {
        if (rank_of(origin) == ColorTraits<~ACTIVE_COLOR>::PAWN_INITIAL_RANK &&
            rank_of(target) == ColorTraits<ACTIVE_COLOR>::PAWN_PROMOTION_RANK) {
          for (const auto promotion : {PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
            test_move(promotion);
          }
        }
      };
      test_move();
      maybe_test_promotion.template operator()<Color::WHITE>();
      maybe_test_promotion.template operator()<Color::BLACK>();
    });
  });
}

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << MOVE;
  BOOST_TEST(os.is_equal("c1c2"));
  os << KNIGHT_PROMOTION;
  BOOST_TEST(os.is_equal("a7a8n"));
  os << BISHOP_PROMOTION;
  BOOST_TEST(os.is_equal("b7b8b"));
  os << ROOK_PROMOTION;
  BOOST_TEST(os.is_equal("c7c8r"));
  os << QUEEN_PROMOTION;
  BOOST_TEST(os.is_equal("d7d8q"));
}
}
}
