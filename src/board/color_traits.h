#pragma once

#include "board/bitboard.h"
#include "board/castling_rights.h"
#include "board/color.h"
#include "board/coordinate.h"
#include "board/direction.h"
#include "board/rank.h"

namespace prodigy::board {
template <Color>
struct ColorTraits;

template <>
struct ColorTraits<Color::WHITE> final {
  static constexpr Rank PAWN_INITIAL_RANK = Rank::TWO;
  static constexpr Rank EN_PASSANT_TARGET_RANK = Rank::THREE;
  static constexpr Rank PAWN_DOUBLE_PUSH_TARGET_RANK = Rank::FOUR;
  static constexpr Rank EN_PASSANT_CAPTURE_ORIGIN_RANK = Rank::FIVE;
  static constexpr Rank PAWN_PROMOTION_RANK = Rank::EIGHT;
  static constexpr Direction RELATIVE_NORTH = Direction::NORTH;
  static constexpr Direction RELATIVE_SOUTH = Direction::SOUTH;
  static constexpr Coordinate KING_INITIAL_ORIGIN = Coordinate::E1;
  static constexpr Coordinate KING_KINGSIDE_CASTLE_TARGET = Coordinate::G1;
  static constexpr Coordinate KING_QUEENSIDE_CASTLE_TARGET = Coordinate::C1;
  static constexpr Coordinate KINGSIDE_ROOK_INITIAL_ORIGIN = Coordinate::H1;
  static constexpr Coordinate QUEENSIDE_ROOK_INITIAL_ORIGIN = Coordinate::A1;
  static constexpr Coordinate KINGSIDE_ROOK_CASTLE_TARGET = Coordinate::F1;
  static constexpr Coordinate QUEENSIDE_ROOK_CASTLE_TARGET = Coordinate::D1;
  static constexpr CastlingRights KINGSIDE_CASTLING_RIGHTS = CastlingRights::WHITE_KINGSIDE;
  static constexpr CastlingRights QUEENSIDE_CASTLING_RIGHTS = CastlingRights::WHITE_QUEENSIDE;
};

template <>
struct ColorTraits<Color::BLACK> final {
  static constexpr Rank PAWN_INITIAL_RANK = Rank::SEVEN;
  static constexpr Rank EN_PASSANT_TARGET_RANK = Rank::SIX;
  static constexpr Rank PAWN_DOUBLE_PUSH_TARGET_RANK = Rank::FIVE;
  static constexpr Rank EN_PASSANT_CAPTURE_ORIGIN_RANK = Rank::FOUR;
  static constexpr Rank PAWN_PROMOTION_RANK = Rank::ONE;
  static constexpr Direction RELATIVE_NORTH = Direction::SOUTH;
  static constexpr Direction RELATIVE_SOUTH = Direction::NORTH;
  static constexpr Coordinate KING_INITIAL_ORIGIN = Coordinate::E8;
  static constexpr Coordinate KING_KINGSIDE_CASTLE_TARGET = Coordinate::G8;
  static constexpr Coordinate KING_QUEENSIDE_CASTLE_TARGET = Coordinate::C8;
  static constexpr Coordinate KINGSIDE_ROOK_INITIAL_ORIGIN = Coordinate::H8;
  static constexpr Coordinate QUEENSIDE_ROOK_INITIAL_ORIGIN = Coordinate::A8;
  static constexpr Coordinate KINGSIDE_ROOK_CASTLE_TARGET = Coordinate::F8;
  static constexpr Coordinate QUEENSIDE_ROOK_CASTLE_TARGET = Coordinate::D8;
  static constexpr CastlingRights KINGSIDE_CASTLING_RIGHTS = CastlingRights::BLACK_KINGSIDE;
  static constexpr CastlingRights QUEENSIDE_CASTLING_RIGHTS = CastlingRights::BLACK_QUEENSIDE;
};
}
