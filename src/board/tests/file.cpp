#define BOOST_TEST_MODULE File

#include "board/file.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

namespace prodigy::board {
namespace {
static_assert(sizeof(File) == 1);

static_assert(to_file('a') == File::A);
static_assert(to_file('b') == File::B);
static_assert(to_file('c') == File::C);
static_assert(to_file('d') == File::D);
static_assert(to_file('e') == File::E);
static_assert(to_file('f') == File::F);
static_assert(to_file('g') == File::G);
static_assert(to_file('h') == File::H);

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << File::A;
  BOOST_TEST(os.is_equal("a"));
  os << File::B;
  BOOST_TEST(os.is_equal("b"));
  os << File::C;
  BOOST_TEST(os.is_equal("c"));
  os << File::D;
  BOOST_TEST(os.is_equal("d"));
  os << File::E;
  BOOST_TEST(os.is_equal("e"));
  os << File::F;
  BOOST_TEST(os.is_equal("f"));
  os << File::G;
  BOOST_TEST(os.is_equal("g"));
  os << File::H;
  BOOST_TEST(os.is_equal("h"));
}
}
}
