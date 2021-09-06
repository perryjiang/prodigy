#define BOOST_TEST_MODULE Color

#include "board/color.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

namespace prodigy::board {
namespace {
static_assert(sizeof(Color) == 1);

static_assert(to_color('w') == Color::WHITE);
static_assert(to_color('b') == Color::BLACK);

static_assert(~Color::WHITE == Color::BLACK);
static_assert(~Color::BLACK == Color::WHITE);

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << Color::WHITE;
  BOOST_TEST(os.is_equal("w"));
  os << Color::BLACK;
  BOOST_TEST(os.is_equal("b"));
}
}
}
