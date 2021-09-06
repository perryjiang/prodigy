#pragma once

#include "board/bitboard.h"
#include "board/piece_map.h"

namespace prodigy::board {
using Board = PieceMap<Bitboard>;
}
