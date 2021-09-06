#define BOOST_TEST_MODULE NodeType

#include "search/node_type.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

namespace prodigy::search {
namespace {
static_assert(sizeof(NodeType) == 1);

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << NodeType::PV;
  BOOST_TEST(os.is_equal("PV"));
  os << NodeType::CUT;
  BOOST_TEST(os.is_equal("CUT"));
  os << NodeType::ALL;
  BOOST_TEST(os.is_equal("ALL"));
}
}
}
