#define BOOST_TEST_MODULE Rank

#include "board/rank.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

namespace prodigy::board {
namespace {
static_assert(sizeof(Rank) == 1);

static_assert(to_rank('1') == Rank::ONE);
static_assert(to_rank('2') == Rank::TWO);
static_assert(to_rank('3') == Rank::THREE);
static_assert(to_rank('4') == Rank::FOUR);
static_assert(to_rank('5') == Rank::FIVE);
static_assert(to_rank('6') == Rank::SIX);
static_assert(to_rank('7') == Rank::SEVEN);
static_assert(to_rank('8') == Rank::EIGHT);

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << Rank::ONE;
  BOOST_TEST(os.is_equal("1"));
  os << Rank::TWO;
  BOOST_TEST(os.is_equal("2"));
  os << Rank::THREE;
  BOOST_TEST(os.is_equal("3"));
  os << Rank::FOUR;
  BOOST_TEST(os.is_equal("4"));
  os << Rank::FIVE;
  BOOST_TEST(os.is_equal("5"));
  os << Rank::SIX;
  BOOST_TEST(os.is_equal("6"));
  os << Rank::SEVEN;
  BOOST_TEST(os.is_equal("7"));
  os << Rank::EIGHT;
  BOOST_TEST(os.is_equal("8"));
}
}
}
