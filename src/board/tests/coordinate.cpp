#define BOOST_TEST_MODULE Coordinate

#include "board/coordinate.h"

#include <array>
#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <tuple>

#include "board/file.h"
#include "board/rank.h"

namespace prodigy::board {
namespace {
static_assert(sizeof(Coordinate) == 1);

constexpr std::array<std::tuple<Coordinate, File, Rank>, 64> DECOMPOSED_COORDINATES = {{
    {Coordinate::A1, File::A, Rank::ONE},   {Coordinate::B1, File::B, Rank::ONE},
    {Coordinate::C1, File::C, Rank::ONE},   {Coordinate::D1, File::D, Rank::ONE},
    {Coordinate::E1, File::E, Rank::ONE},   {Coordinate::F1, File::F, Rank::ONE},
    {Coordinate::G1, File::G, Rank::ONE},   {Coordinate::H1, File::H, Rank::ONE},
    {Coordinate::A2, File::A, Rank::TWO},   {Coordinate::B2, File::B, Rank::TWO},
    {Coordinate::C2, File::C, Rank::TWO},   {Coordinate::D2, File::D, Rank::TWO},
    {Coordinate::E2, File::E, Rank::TWO},   {Coordinate::F2, File::F, Rank::TWO},
    {Coordinate::G2, File::G, Rank::TWO},   {Coordinate::H2, File::H, Rank::TWO},
    {Coordinate::A3, File::A, Rank::THREE}, {Coordinate::B3, File::B, Rank::THREE},
    {Coordinate::C3, File::C, Rank::THREE}, {Coordinate::D3, File::D, Rank::THREE},
    {Coordinate::E3, File::E, Rank::THREE}, {Coordinate::F3, File::F, Rank::THREE},
    {Coordinate::G3, File::G, Rank::THREE}, {Coordinate::H3, File::H, Rank::THREE},
    {Coordinate::A4, File::A, Rank::FOUR},  {Coordinate::B4, File::B, Rank::FOUR},
    {Coordinate::C4, File::C, Rank::FOUR},  {Coordinate::D4, File::D, Rank::FOUR},
    {Coordinate::E4, File::E, Rank::FOUR},  {Coordinate::F4, File::F, Rank::FOUR},
    {Coordinate::G4, File::G, Rank::FOUR},  {Coordinate::H4, File::H, Rank::FOUR},
    {Coordinate::A5, File::A, Rank::FIVE},  {Coordinate::B5, File::B, Rank::FIVE},
    {Coordinate::C5, File::C, Rank::FIVE},  {Coordinate::D5, File::D, Rank::FIVE},
    {Coordinate::E5, File::E, Rank::FIVE},  {Coordinate::F5, File::F, Rank::FIVE},
    {Coordinate::G5, File::G, Rank::FIVE},  {Coordinate::H5, File::H, Rank::FIVE},
    {Coordinate::A6, File::A, Rank::SIX},   {Coordinate::B6, File::B, Rank::SIX},
    {Coordinate::C6, File::C, Rank::SIX},   {Coordinate::D6, File::D, Rank::SIX},
    {Coordinate::E6, File::E, Rank::SIX},   {Coordinate::F6, File::F, Rank::SIX},
    {Coordinate::G6, File::G, Rank::SIX},   {Coordinate::H6, File::H, Rank::SIX},
    {Coordinate::A7, File::A, Rank::SEVEN}, {Coordinate::B7, File::B, Rank::SEVEN},
    {Coordinate::C7, File::C, Rank::SEVEN}, {Coordinate::D7, File::D, Rank::SEVEN},
    {Coordinate::E7, File::E, Rank::SEVEN}, {Coordinate::F7, File::F, Rank::SEVEN},
    {Coordinate::G7, File::G, Rank::SEVEN}, {Coordinate::H7, File::H, Rank::SEVEN},
    {Coordinate::A8, File::A, Rank::EIGHT}, {Coordinate::B8, File::B, Rank::EIGHT},
    {Coordinate::C8, File::C, Rank::EIGHT}, {Coordinate::D8, File::D, Rank::EIGHT},
    {Coordinate::E8, File::E, Rank::EIGHT}, {Coordinate::F8, File::F, Rank::EIGHT},
    {Coordinate::G8, File::G, Rank::EIGHT}, {Coordinate::H8, File::H, Rank::EIGHT},
}};

BOOST_AUTO_TEST_CASE(test_to_coordinate) {
  for (const auto& [coordinate, file, rank] : DECOMPOSED_COORDINATES) {
    BOOST_TEST(coordinate == to_coordinate(file, rank));
  }
}

BOOST_AUTO_TEST_CASE(test_file_of) {
  for (const auto& [coordinate, file, rank] : DECOMPOSED_COORDINATES) {
    BOOST_TEST(file == file_of(coordinate));
  }
}

BOOST_AUTO_TEST_CASE(test_rank_of) {
  for (const auto& [coordinate, file, rank] : DECOMPOSED_COORDINATES) {
    BOOST_TEST(rank == rank_of(coordinate));
  }
}

BOOST_AUTO_TEST_CASE(test_for_each_coordinate) {
  auto it = DECOMPOSED_COORDINATES.begin();
  for_each_coordinate([&](const auto coordinate) {
    BOOST_TEST_REQUIRE(it != DECOMPOSED_COORDINATES.end());
    BOOST_TEST(std::get<Coordinate>(*it) == coordinate);
    ++it;
  });
  BOOST_TEST(it == DECOMPOSED_COORDINATES.end());
}

BOOST_AUTO_TEST_CASE(output_stream) {
  for (boost::test_tools::output_test_stream os; const auto& [coordinate, file, rank] : DECOMPOSED_COORDINATES) {
    os << coordinate;
    std::ostringstream expected;
    expected << file << rank;
    BOOST_TEST(os.is_equal(expected.str()));
  }
}
}
}
