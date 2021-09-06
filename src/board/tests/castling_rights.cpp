#define BOOST_TEST_MODULE CastlingRights

#include "board/castling_rights.h"

#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <string>
#include <unordered_map>

namespace prodigy::board {
namespace {
static_assert(sizeof(CastlingRights) == 1);

const std::unordered_map<CastlingRights, std::string> CASTLING_RIGHTS_TO_STRING = {
    {CastlingRights::NONE, "-"},
    {CastlingRights::WHITE_KINGSIDE, "K"},
    {CastlingRights::WHITE_QUEENSIDE, "Q"},
    {CastlingRights::BLACK_KINGSIDE, "k"},
    {CastlingRights::BLACK_QUEENSIDE, "q"},
    {CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE, "KQ"},
    {CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE, "kq"},
    {CastlingRights::WHITE_QUEENSIDE | CastlingRights::BLACK_KINGSIDE, "Qk"},
    {CastlingRights::WHITE_KINGSIDE | CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE, "Kkq"},
    {CastlingRights::ALL, "KQkq"},
};

BOOST_AUTO_TEST_CASE(grant_castling_rights) {
  CastlingRights castling_rights = CastlingRights::NONE;
  const auto grant = [&](const auto rights) {
    BOOST_TEST((castling_rights & rights) == CastlingRights::NONE);
    castling_rights |= rights;
    BOOST_TEST((castling_rights & rights) != CastlingRights::NONE);
  };
  grant(CastlingRights::WHITE_KINGSIDE);
  grant(CastlingRights::WHITE_QUEENSIDE);
  grant(CastlingRights::BLACK_KINGSIDE);
  grant(CastlingRights::BLACK_QUEENSIDE);
  BOOST_TEST(castling_rights == CastlingRights::ALL);
}

BOOST_AUTO_TEST_CASE(revoke_castling_rights) {
  CastlingRights castling_rights = CastlingRights::ALL;
  const auto revoke = [&](const auto rights) {
    BOOST_TEST((castling_rights & rights) != CastlingRights::NONE);
    castling_rights &= ~rights;
    BOOST_TEST((castling_rights & rights) == CastlingRights::NONE);
  };
  revoke(CastlingRights::WHITE_KINGSIDE);
  revoke(CastlingRights::WHITE_QUEENSIDE);
  revoke(CastlingRights::BLACK_KINGSIDE);
  revoke(CastlingRights::BLACK_QUEENSIDE);
  BOOST_TEST(castling_rights == CastlingRights::NONE);
}

BOOST_AUTO_TEST_CASE(test_to_castling_rights) {
  for (const auto& [castling_rights, str] : CASTLING_RIGHTS_TO_STRING) {
    BOOST_TEST(castling_rights == to_castling_rights(str));
  }
}

BOOST_AUTO_TEST_CASE(output_stream) {
  for (boost::test_tools::output_test_stream os; const auto& [castling_rights, str] : CASTLING_RIGHTS_TO_STRING) {
    os << castling_rights;
    BOOST_TEST(os.is_equal(str));
  }
}
}
}
