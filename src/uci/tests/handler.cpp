#define BOOST_TEST_MODULE Handler

#include "uci/handler.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <memory>

#include "search/searcher.h"

namespace prodigy::uci {
namespace {
class NoopSearcher final : public search::Searcher {
 private:
  std::optional<board::Move> search(const board::Position&) override { return std::nullopt; }
};

struct Fixture {
  boost::test_tools::output_test_stream output;
  Handler handler{output, std::make_unique<NoopSearcher>()};
};

BOOST_FIXTURE_TEST_SUITE(Handle, Fixture)

BOOST_AUTO_TEST_CASE(uci) {
  BOOST_TEST(handler.handle("uci"));
  BOOST_TEST(
      output.is_equal("id name Prodigy\n"
                      "id author Perry Jiang\n"
                      "uciok\n"));
}

BOOST_AUTO_TEST_CASE(debug) {
  BOOST_TEST(handler.handle("debug on"));
  BOOST_TEST(output.is_empty());
  BOOST_TEST(handler.handle("debug off"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_CASE(isready) {
  BOOST_TEST(handler.handle("isready"));
  BOOST_TEST(output.is_equal("readyok\n"));
}

BOOST_AUTO_TEST_CASE(setoption) {
  BOOST_TEST(handler.handle("setoption name foo value bar"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_CASE(ucinewgame) {
  BOOST_TEST(handler.handle("ucinewgame"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_CASE(position) {
  BOOST_TEST(handler.handle("position startpos"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_CASE(stop) {
  BOOST_TEST(handler.handle("stop"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_CASE(ponderhit) {
  BOOST_TEST(handler.handle("ponderhit"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_CASE(quit) {
  BOOST_TEST(!handler.handle("quit"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_CASE(unhandled) {
  BOOST_TEST(handler.handle("unknown message"));
  BOOST_TEST(output.is_empty());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(go) {
  boost::test_tools::output_test_stream output;
  {
    Handler handler(output, std::make_unique<NoopSearcher>());
    BOOST_TEST(handler.handle("position startpos"));
    BOOST_TEST(handler.handle("go"));
  }
  BOOST_TEST(output.is_equal("bestmove 0000\n"));
}
}
}
