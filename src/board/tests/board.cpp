#define BOOST_TEST_MODULE Board

#include "board/board.h"

#include <boost/test/unit_test.hpp>

#include "board/bitboard.h"
#include "board/color.h"
#include "board/coordinate.h"
#include "board/piece_type.h"

namespace prodigy::board {
namespace {
BOOST_AUTO_TEST_CASE(get) {
  const auto board = [] {
    Board board;
    BOOST_TEST((board.get<Color::WHITE, PieceType::PAWN>()) == Bitboard());
    BOOST_TEST((board.get<Color::WHITE, PieceType::KNIGHT>()) == Bitboard());
    BOOST_TEST((board.get<Color::WHITE, PieceType::BISHOP>()) == Bitboard());
    BOOST_TEST((board.get<Color::WHITE, PieceType::ROOK>()) == Bitboard());
    BOOST_TEST((board.get<Color::WHITE, PieceType::QUEEN>()) == Bitboard());
    BOOST_TEST((board.get<Color::WHITE, PieceType::KING>()) == Bitboard());
    BOOST_TEST((board.get<Color::BLACK, PieceType::PAWN>()) == Bitboard());
    BOOST_TEST((board.get<Color::BLACK, PieceType::KNIGHT>()) == Bitboard());
    BOOST_TEST((board.get<Color::BLACK, PieceType::BISHOP>()) == Bitboard());
    BOOST_TEST((board.get<Color::BLACK, PieceType::ROOK>()) == Bitboard());
    BOOST_TEST((board.get<Color::BLACK, PieceType::QUEEN>()) == Bitboard());
    BOOST_TEST((board.get<Color::BLACK, PieceType::KING>()) == Bitboard());

    board.get<Color::WHITE, PieceType::PAWN>() = Bitboard(Coordinate::A1);
    board.get<Color::WHITE, PieceType::KNIGHT>() = Bitboard(Coordinate::B1);
    board.get<Color::WHITE, PieceType::BISHOP>() = Bitboard(Coordinate::C1);
    board.get<Color::WHITE, PieceType::ROOK>() = Bitboard(Coordinate::D1);
    board.get<Color::WHITE, PieceType::QUEEN>() = Bitboard(Coordinate::E1);
    board.get<Color::WHITE, PieceType::KING>() = Bitboard(Coordinate::F1);
    board.get<Color::BLACK, PieceType::PAWN>() = Bitboard(Coordinate::A8);
    board.get<Color::BLACK, PieceType::KNIGHT>() = Bitboard(Coordinate::B8);
    board.get<Color::BLACK, PieceType::BISHOP>() = Bitboard(Coordinate::C8);
    board.get<Color::BLACK, PieceType::ROOK>() = Bitboard(Coordinate::D8);
    board.get<Color::BLACK, PieceType::QUEEN>() = Bitboard(Coordinate::E8);
    board.get<Color::BLACK, PieceType::KING>() = Bitboard(Coordinate::F8);
    return board;
  }();

  BOOST_TEST((board.get<Color::WHITE, PieceType::PAWN>()) == Bitboard(Coordinate::A1));
  BOOST_TEST((board.get<Color::WHITE, PieceType::KNIGHT>()) == Bitboard(Coordinate::B1));
  BOOST_TEST((board.get<Color::WHITE, PieceType::BISHOP>()) == Bitboard(Coordinate::C1));
  BOOST_TEST((board.get<Color::WHITE, PieceType::ROOK>()) == Bitboard(Coordinate::D1));
  BOOST_TEST((board.get<Color::WHITE, PieceType::QUEEN>()) == Bitboard(Coordinate::E1));
  BOOST_TEST((board.get<Color::WHITE, PieceType::KING>()) == Bitboard(Coordinate::F1));
  BOOST_TEST((board.get<Color::BLACK, PieceType::PAWN>()) == Bitboard(Coordinate::A8));
  BOOST_TEST((board.get<Color::BLACK, PieceType::KNIGHT>()) == Bitboard(Coordinate::B8));
  BOOST_TEST((board.get<Color::BLACK, PieceType::BISHOP>()) == Bitboard(Coordinate::C8));
  BOOST_TEST((board.get<Color::BLACK, PieceType::ROOK>()) == Bitboard(Coordinate::D8));
  BOOST_TEST((board.get<Color::BLACK, PieceType::QUEEN>()) == Bitboard(Coordinate::E8));
  BOOST_TEST((board.get<Color::BLACK, PieceType::KING>()) == Bitboard(Coordinate::F8));
}
}
}
