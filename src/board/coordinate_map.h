#pragma once

#include "base/enum_map.h"
#include "board/coordinate.h"

namespace prodigy::board {
template <typename T>
using CoordinateMap = EnumMap<Coordinate, T, 64>;
}
