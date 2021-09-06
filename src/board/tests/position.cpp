#define BOOST_TEST_MODULE Position

#include "board/position.h"

#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "base/ply.h"
#include "board/bitboard.h"
#include "board/castling_rights.h"
#include "board/color.h"
#include "board/coordinate.h"
#include "board/move.h"
#include "board/piece_type.h"
#include "board/rank.h"
#include "board/starting_position_fen.h"

namespace prodigy::board {
template <typename T>
std::ostream& boost_test_print_type(std::ostream& os, const std::optional<T>& opt) {
  if (opt.has_value()) {
    os << *opt;
  } else {
    os << "nullopt";
  }
  return os;
}

namespace {
static_assert(std::is_trivially_copyable_v<Position>);

Position test_move(const Position& position_before_move, const Move move, const CastlingRights expected_castling_rights,
                   const std::optional<Coordinate> expected_en_passant_target = std::nullopt) {
  const auto apply_and_validate =
      [&]<Color ACTIVE_COLOR> {
        const auto position_after_move = position_before_move.apply<ACTIVE_COLOR>(move);
        BOOST_TEST_CONTEXT("BEFORE MOVE:\n"
                               << position_before_move,
                           "AFTER MOVE:\n"
                               << position_after_move,
                           "MOVE: " << move) {
          const auto piece_type_before_move = position_before_move.piece_type_at<ACTIVE_COLOR>(move.origin());
          const auto piece_type_after_move = position_after_move.template piece_type_at<ACTIVE_COLOR>(move.target());
          BOOST_TEST_REQUIRE(piece_type_before_move.has_value());
          BOOST_TEST_REQUIRE(piece_type_after_move.has_value());
          if (const auto promotion = move.promotion(); promotion.has_value()) {
            BOOST_TEST_REQUIRE(piece_type_before_move == PieceType::PAWN);
            BOOST_TEST(piece_type_after_move == promotion);
          } else {
            BOOST_TEST(piece_type_after_move == piece_type_before_move);
          }

          BOOST_TEST(position_after_move.active_color() == ~position_before_move.active_color());
          BOOST_TEST(position_after_move.castling_rights() == expected_castling_rights);
          BOOST_TEST(position_after_move.en_passant_target() == expected_en_passant_target);

          if (*piece_type_before_move == PieceType::PAWN ||
              position_before_move.piece_type_at<~ACTIVE_COLOR>(move.target()).has_value()) {
            BOOST_TEST(!position_after_move.halfmove_clock());
          } else {
            BOOST_TEST(position_after_move.halfmove_clock() == position_before_move.halfmove_clock() + 1);
          }

          if (position_before_move.active_color() == Color::WHITE) {
            BOOST_TEST(position_after_move.fullmove_number() == position_before_move.fullmove_number());
          } else {
            BOOST_TEST(position_after_move.fullmove_number() == position_before_move.fullmove_number() + 1);
          }
          BOOST_TEST(position_after_move.hash() == Position::from_fen(position_after_move.fen()).hash());
        }
        return position_after_move;
      };
  return position_before_move.active_color() == Color::WHITE ? apply_and_validate.template operator()<Color::WHITE>()
                                                             : apply_and_validate.template operator()<Color::BLACK>();
}

Position test_move(const std::string_view fen, const Move move, const CastlingRights expected_castling_rights,
                   const std::optional<Coordinate> expected_en_passant_target = std::nullopt) {
  return test_move(Position::from_fen(fen), move, expected_castling_rights, expected_en_passant_target);
}

BOOST_AUTO_TEST_CASE(from_fen) {
  struct PiecelessPosition {
    Color active_color;
    CastlingRights castling_rights;
    std::optional<Coordinate> en_passant_target;
    Ply halfmove_clock;
    std::uint16_t fullmove_number;
  };

  static const std::unordered_map<std::string_view, PiecelessPosition> FEN_TO_PIECELESS_POSITION = {
      {
          STARTING_POSITION_FEN,
          {
              Color::WHITE,
              CastlingRights::ALL,
              std::nullopt,
              0,
              1,
          },
      },
      {
          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 3 12",
          {
              Color::WHITE,
              CastlingRights::ALL,
              std::nullopt,
              3,
              12,
          },
      },
      {
          "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 20 40",
          {
              Color::WHITE,
              CastlingRights::NONE,
              std::nullopt,
              20,
              40,
          },
      },
      {
          "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",
          {
              Color::BLACK,
              CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE,
              std::nullopt,
              0,
              1,
          },
      },
      {
          "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
          {
              Color::BLACK,
              CastlingRights::ALL,
              Coordinate::E3,
              0,
              1,
          },
      },
  };

  for (const auto& [fen, pieceless_position] : FEN_TO_PIECELESS_POSITION) {
    const auto position = Position::from_fen(fen);
    const auto& [active_color, castling_rights, en_passant_target, halfmove_clock, fullmove_number] =
        pieceless_position;
    BOOST_TEST(position.active_color() == active_color);
    BOOST_TEST(position.castling_rights() == castling_rights);
    BOOST_TEST(position.en_passant_target() == en_passant_target);
    BOOST_TEST(position.halfmove_clock() == halfmove_clock);
    BOOST_TEST(position.fullmove_number() == fullmove_number);
  }
}

BOOST_AUTO_TEST_CASE(pieces) {
  BOOST_TEST(Position::starting_position().all_pieces<Color::WHITE>() == [] {
    Bitboard bitboard;
    for_each_coordinate([&](const auto coordinate) {
      if (rank_of(coordinate) == Rank::ONE || rank_of(coordinate) == Rank::TWO) {
        bitboard |= Bitboard(coordinate);
      }
    });
    return bitboard;
  }());
  BOOST_TEST(Position::starting_position().all_pieces<Color::BLACK>() == [] {
    Bitboard bitboard;
    for_each_coordinate([&](const auto coordinate) {
      if (rank_of(coordinate) == Rank::SEVEN || rank_of(coordinate) == Rank::EIGHT) {
        bitboard |= Bitboard(coordinate);
      }
    });
    return bitboard;
  }());

  BOOST_TEST((Position::starting_position().pieces<Color::WHITE, PieceType::PAWN>()) == [] {
    Bitboard bitboard;
    for_each_coordinate([&](const auto coordinate) {
      if (rank_of(coordinate) == Rank::TWO) {
        bitboard |= Bitboard(coordinate);
      }
    });
    return bitboard;
  }());
  BOOST_TEST((Position::starting_position().pieces<Color::WHITE, PieceType::KNIGHT>()) ==
             (Bitboard(Coordinate::B1) | Bitboard(Coordinate::G1)));
  BOOST_TEST((Position::starting_position().pieces<Color::WHITE, PieceType::BISHOP>()) ==
             (Bitboard(Coordinate::C1) | Bitboard(Coordinate::F1)));
  BOOST_TEST((Position::starting_position().pieces<Color::WHITE, PieceType::ROOK>()) ==
             (Bitboard(Coordinate::A1) | Bitboard(Coordinate::H1)));
  BOOST_TEST((Position::starting_position().pieces<Color::WHITE, PieceType::QUEEN>()) == Bitboard(Coordinate::D1));
  BOOST_TEST((Position::starting_position().pieces<Color::WHITE, PieceType::KING>()) == Bitboard(Coordinate::E1));

  BOOST_TEST((Position::starting_position().pieces<Color::BLACK, PieceType::PAWN>()) == [] {
    Bitboard bitboard;
    for_each_coordinate([&](const auto coordinate) {
      if (rank_of(coordinate) == Rank::SEVEN) {
        bitboard |= Bitboard(coordinate);
      }
    });
    return bitboard;
  }());
  BOOST_TEST((Position::starting_position().pieces<Color::BLACK, PieceType::KNIGHT>()) ==
             (Bitboard(Coordinate::B8) | Bitboard(Coordinate::G8)));
  BOOST_TEST((Position::starting_position().pieces<Color::BLACK, PieceType::BISHOP>()) ==
             (Bitboard(Coordinate::C8) | Bitboard(Coordinate::F8)));
  BOOST_TEST((Position::starting_position().pieces<Color::BLACK, PieceType::ROOK>()) ==
             (Bitboard(Coordinate::A8) | Bitboard(Coordinate::H8)));
  BOOST_TEST((Position::starting_position().pieces<Color::BLACK, PieceType::QUEEN>()) == Bitboard(Coordinate::D8));
  BOOST_TEST((Position::starting_position().pieces<Color::BLACK, PieceType::KING>()) == Bitboard(Coordinate::E8));

  BOOST_TEST((Position::starting_position().pieces<Color::WHITE, PieceType::BISHOP, PieceType::QUEEN>()) ==
             (Bitboard(Coordinate::C1) | Bitboard(Coordinate::F1) | Bitboard(Coordinate::D1)));
  BOOST_TEST((Position::starting_position().pieces<Color::BLACK, PieceType::ROOK, PieceType::QUEEN>()) ==
             (Bitboard(Coordinate::A8) | Bitboard(Coordinate::H8) | Bitboard(Coordinate::D8)));
}

BOOST_AUTO_TEST_CASE(pawn_single_push) {
  test_move(Position::starting_position(), Move(Coordinate::E2, Coordinate::E3), CastlingRights::ALL);
}

BOOST_AUTO_TEST_CASE(pawn_double_push) {
  test_move(Position::starting_position(), Move(Coordinate::E2, Coordinate::E4), CastlingRights::ALL, Coordinate::E3);
}

BOOST_AUTO_TEST_CASE(pawn_normal_capture) {
  test_move("rnbqkbnr/pppp1ppp/8/4p3/3N4/8/PPPPPPPP/RNBQKB1R b KQkq - 1 2", Move(Coordinate::E5, Coordinate::D4),
            CastlingRights::ALL);
}

BOOST_AUTO_TEST_CASE(en_passant_capture) {
  test_move("rnbqkbnr/ppp1pppp/8/8/3pP1PP/8/PPPP1P2/RNBQKBNR b KQkq e3 0 2", Move(Coordinate::D4, Coordinate::E3),
            CastlingRights::ALL);
}

BOOST_AUTO_TEST_CASE(pawn_promotion) {
  for (const auto promotion : {PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
    test_move("8/k7/8/8/8/8/K6p/8 b - - 0 1", Move(Coordinate::H2, Coordinate::H1, promotion), CastlingRights::NONE);
  }
}

BOOST_AUTO_TEST_CASE(king_move_revokes_castling_rights) {
  const auto position_after_white_king_move =
      test_move("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2", Move(Coordinate::E1, Coordinate::E2),
                CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
  test_move(position_after_white_king_move, Move(Coordinate::E8, Coordinate::E7), CastlingRights::NONE);
}

BOOST_AUTO_TEST_CASE(rook_move_revokes_castling_rights) {
  const auto position_after_white_kingside_rook_move =
      test_move("rnbqkbnr/1pppppp1/p6p/8/8/P6P/1PPPPPP1/RNBQKBNR w KQkq - 0 3", Move(Coordinate::H1, Coordinate::H2),
                CastlingRights::ALL & ~CastlingRights::WHITE_KINGSIDE);
  const auto position_after_black_kingside_rook_move =
      test_move(position_after_white_kingside_rook_move, Move(Coordinate::H8, Coordinate::H7),
                CastlingRights::WHITE_QUEENSIDE | CastlingRights::BLACK_QUEENSIDE);
  const auto position_after_white_queenside_rook_move = test_move(
      position_after_black_kingside_rook_move, Move(Coordinate::A1, Coordinate::A2), CastlingRights::BLACK_QUEENSIDE);
  test_move(position_after_white_queenside_rook_move, Move(Coordinate::A8, Coordinate::A7), CastlingRights::NONE);
}

BOOST_AUTO_TEST_CASE(captured_rook_revokes_castling_rights) {
  const auto position_after_black_kingside_rook_captured =
      test_move("r1bqkb1r/pppppppp/1N4N1/8/8/1n4n1/PPPPPPPP/R1BQKB1R w KQkq - 12 6",
                Move(Coordinate::G6, Coordinate::H8), CastlingRights::ALL & ~CastlingRights::BLACK_KINGSIDE);
  const auto position_after_white_kingside_rook_captured =
      test_move(position_after_black_kingside_rook_captured, Move(Coordinate::G3, Coordinate::H1),
                CastlingRights::WHITE_QUEENSIDE | CastlingRights::BLACK_QUEENSIDE);
  const auto position_after_black_queenside_rook_captured =
      test_move(position_after_white_kingside_rook_captured, Move(Coordinate::B6, Coordinate::A8),
                CastlingRights::WHITE_QUEENSIDE);
  test_move(position_after_black_queenside_rook_captured, Move(Coordinate::B3, Coordinate::A1), CastlingRights::NONE);
}

BOOST_AUTO_TEST_CASE(castling_revokes_castling_rights) {
  static constexpr auto FEN = "r3k2r/pppbqppp/2nb1n2/3pp3/3PP3/2N1BN2/PPPQBPPP/R3K2R w KQkq - 6 12";
  const auto position_after_white_kingside_castle = test_move(
      FEN, Move(Coordinate::E1, Coordinate::G1), CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
  BOOST_TEST_CONTEXT(position_after_white_kingside_castle) {
    BOOST_TEST(!position_after_white_kingside_castle.piece_type_at<Color::WHITE>(Coordinate::H1).has_value());
    BOOST_TEST(position_after_white_kingside_castle.piece_type_at<Color::WHITE>(Coordinate::F1) == PieceType::ROOK);
  }

  const auto position_after_black_kingside_castle =
      test_move(position_after_white_kingside_castle, Move(Coordinate::E8, Coordinate::G8), CastlingRights::NONE);
  BOOST_TEST_CONTEXT(position_after_black_kingside_castle) {
    BOOST_TEST(!position_after_black_kingside_castle.piece_type_at<Color::BLACK>(Coordinate::H8).has_value());
    BOOST_TEST(position_after_black_kingside_castle.piece_type_at<Color::BLACK>(Coordinate::F8) == PieceType::ROOK);
  }

  const auto position_after_white_queenside_castle = test_move(
      FEN, Move(Coordinate::E1, Coordinate::C1), CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
  BOOST_TEST_CONTEXT(position_after_white_queenside_castle) {
    BOOST_TEST(!position_after_white_queenside_castle.piece_type_at<Color::WHITE>(Coordinate::A1).has_value());
    BOOST_TEST(position_after_white_queenside_castle.piece_type_at<Color::WHITE>(Coordinate::D1) == PieceType::ROOK);
  }

  const auto position_after_black_queenside_castle =
      test_move(position_after_white_queenside_castle, Move(Coordinate::E8, Coordinate::C8), CastlingRights::NONE);
  BOOST_TEST_CONTEXT(position_after_black_queenside_castle) {
    BOOST_TEST(!position_after_black_queenside_castle.piece_type_at<Color::BLACK>(Coordinate::A8).has_value());
    BOOST_TEST(position_after_black_queenside_castle.piece_type_at<Color::BLACK>(Coordinate::D8) == PieceType::ROOK);
  }
}

BOOST_AUTO_TEST_CASE(halfmove_clock) {
  const auto expected_castling_rights = CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE;
  const auto position_after_castle = test_move("rnbqk2r/pppp1ppp/5n2/2b1p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1",
                                               Move(Coordinate::E1, Coordinate::G1), expected_castling_rights);
  const auto position_after_capture =
      test_move(position_after_castle, Move(Coordinate::F6, Coordinate::E4), expected_castling_rights);
  const auto position_after_non_pawn_quiet_move =
      test_move(position_after_capture, Move(Coordinate::F1, Coordinate::E1), expected_castling_rights);
  test_move(position_after_non_pawn_quiet_move, Move(Coordinate::H7, Coordinate::H6), expected_castling_rights);
}

BOOST_AUTO_TEST_CASE(fullmove_number) {
  const auto position_after_white_move =
      test_move(Position::starting_position(), Move(Coordinate::E2, Coordinate::E3), CastlingRights::ALL);
  test_move(position_after_white_move, Move(Coordinate::E7, Coordinate::E5), CastlingRights::ALL, Coordinate::E6);
}

BOOST_AUTO_TEST_CASE(same_zobrist_hash_after_undo) {
  BOOST_TEST(Position::starting_position()
                 .apply<Color::WHITE>(Move(Coordinate::G1, Coordinate::F3))
                 .apply<Color::BLACK>(Move(Coordinate::G8, Coordinate::F6))
                 .apply<Color::WHITE>(Move(Coordinate::F3, Coordinate::G1))
                 .apply<Color::BLACK>(Move(Coordinate::F6, Coordinate::G8))
                 .hash() == Position::starting_position().hash());
}

BOOST_AUTO_TEST_CASE(transposition_zobrist_hash) {
  BOOST_TEST(Position::starting_position()
                 .apply<Color::WHITE>(Move(Coordinate::E2, Coordinate::E3))
                 .apply<Color::BLACK>(Move(Coordinate::E7, Coordinate::E6))
                 .apply<Color::WHITE>(Move(Coordinate::G1, Coordinate::F3))
                 .apply<Color::BLACK>(Move(Coordinate::G8, Coordinate::F6))
                 .hash() == Position::starting_position()
                                .apply<Color::WHITE>(Move(Coordinate::G1, Coordinate::F3))
                                .apply<Color::BLACK>(Move(Coordinate::G8, Coordinate::F6))
                                .apply<Color::WHITE>(Move(Coordinate::E2, Coordinate::E3))
                                .apply<Color::BLACK>(Move(Coordinate::E7, Coordinate::E6))
                                .hash());

  BOOST_TEST(Position::starting_position()
                 .apply<Color::WHITE>(Move(Coordinate::E2, Coordinate::E4))
                 .apply<Color::BLACK>(Move(Coordinate::E7, Coordinate::E5))
                 .apply<Color::WHITE>(Move(Coordinate::G1, Coordinate::F3))
                 .apply<Color::BLACK>(Move(Coordinate::G8, Coordinate::F6))
                 .hash() != Position::starting_position()
                                .apply<Color::WHITE>(Move(Coordinate::G1, Coordinate::F3))
                                .apply<Color::BLACK>(Move(Coordinate::G8, Coordinate::F6))
                                .apply<Color::WHITE>(Move(Coordinate::E2, Coordinate::E4))
                                .apply<Color::BLACK>(Move(Coordinate::E7, Coordinate::E5))
                                .hash());
}
}
}
