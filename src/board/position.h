#pragma once

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

#include "base/ply.h"
#include "board/bitboard.h"
#include "board/board.h"
#include "board/castling_rights.h"
#include "board/color.h"
#include "board/coordinate.h"
#include "board/move.h"
#include "board/piece_type.h"
#include "zobrist/hash.h"

namespace prodigy::board {
class Position final {
 public:
  static Position from_fen(std::string_view);

  static const Position& starting_position();

  template <Color COLOR>
  constexpr Bitboard all_pieces() const {
    return pieces<COLOR, PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN,
                  PieceType::KING>();
  }

  template <Color COLOR, PieceType... PIECE_TYPES>
  constexpr Bitboard pieces() const {
    return (board_.get<COLOR, PIECE_TYPES>() | ...);
  }

  constexpr Color active_color() const { return active_color_; }

  constexpr CastlingRights castling_rights() const { return castling_rights_; }

  constexpr std::optional<Coordinate> en_passant_target() const { return en_passant_target_; }

  constexpr Ply halfmove_clock() const { return halfmove_clock_; }

  constexpr std::uint16_t fullmove_number() const { return fullmove_number_; }

  constexpr zobrist::Hash hash() const { return hash_; }

  std::string fen() const;

  template <Color ACTIVE_COLOR>
  [[nodiscard]] Position apply(Move) const;

  template <Color COLOR>
  constexpr std::optional<PieceType> piece_type_at(const Coordinate coordinate) const {
    return piece_type_at<COLOR>(Bitboard(coordinate));
  }

 private:
  constexpr Position(const Board& board, const Color active_color, const CastlingRights castling_rights,
                     const std::optional<Coordinate> en_passant_target, const Ply halfmove_clock,
                     const std::uint16_t fullmove_number, const zobrist::Hash hash)
      : board_(board),
        active_color_(active_color),
        castling_rights_(castling_rights),
        en_passant_target_(en_passant_target),
        halfmove_clock_(halfmove_clock),
        fullmove_number_(fullmove_number),
        hash_(hash) {}

  Position(const Board&, Color active_color, CastlingRights, std::optional<Coordinate> en_passant_target,
           Ply halfmove_clock, std::uint16_t fullmove_number);

  // TODO: consider an auxillary CoordinateMap member variable which will allow this to only require a single lookup.
  template <Color COLOR>
  constexpr std::optional<PieceType> piece_type_at(const Bitboard mask) const {
    if (board_.get<COLOR, PieceType::PAWN>() & mask) {
      return PieceType::PAWN;
    }
    if (board_.get<COLOR, PieceType::KNIGHT>() & mask) {
      return PieceType::KNIGHT;
    }
    if (board_.get<COLOR, PieceType::BISHOP>() & mask) {
      return PieceType::BISHOP;
    }
    if (board_.get<COLOR, PieceType::ROOK>() & mask) {
      return PieceType::ROOK;
    }
    if (board_.get<COLOR, PieceType::QUEEN>() & mask) {
      return PieceType::QUEEN;
    }
    if (board_.get<COLOR, PieceType::KING>() & mask) {
      return PieceType::KING;
    }
    return std::nullopt;
  }

  Board board_;
  Color active_color_;
  CastlingRights castling_rights_;
  std::optional<Coordinate> en_passant_target_;
  Ply halfmove_clock_;
  std::uint16_t fullmove_number_;
  zobrist::Hash hash_;
};

std::ostream& operator<<(std::ostream&, const Position&);
}
