#include "zobrist/randoms.h"

#include <type_traits>

#include "base/uniform_distribution.h"

namespace prodigy::zobrist {
namespace {
template <typename T>
auto make_uniform_randoms() {
  std::remove_const_t<T> randoms;
  for (auto& random : randoms) {
    random = uniform_distribution<Hash>();
  }
  return randoms;
}
}

Randoms::Randoms()
    : piece_to_coordinate_to_random_([] {
        std::remove_const_t<decltype(piece_to_coordinate_to_random_)> randoms;
        board::for_each_coordinate([&](const auto coordinate) {
#define _(COLOR, PIECE_TYPE) \
  randoms.get<board::Color::COLOR, board::PieceType::PIECE_TYPE>()[coordinate] = uniform_distribution<Hash>()
          _(WHITE, PAWN);
          _(WHITE, KNIGHT);
          _(WHITE, BISHOP);
          _(WHITE, ROOK);
          _(WHITE, QUEEN);
          _(WHITE, KING);
          _(BLACK, PAWN);
          _(BLACK, KNIGHT);
          _(BLACK, BISHOP);
          _(BLACK, ROOK);
          _(BLACK, QUEEN);
          _(BLACK, KING);
#undef _
        });
        return randoms;
      }()),
      active_color_random_(uniform_distribution<Hash>()),
      castling_rights_to_random_(make_uniform_randoms<decltype(castling_rights_to_random_)>()),
      en_passant_file_to_random_(make_uniform_randoms<decltype(en_passant_file_to_random_)>()) {
}

const Randoms& Randoms::instance() {
  static const Randoms RANDOMS;
  return RANDOMS;
}

Hash Randoms::active_color_random() const { return active_color_random_; }

Hash Randoms::castling_rights_random(const board::CastlingRights castling_rights) const {
  return castling_rights_to_random_[castling_rights];
}

Hash Randoms::en_passant_file_random(const board::File en_passant_file) const {
  return en_passant_file_to_random_[en_passant_file];
}
}
