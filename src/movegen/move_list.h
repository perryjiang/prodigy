#pragma once

#include <boost/container/static_vector.hpp>

#include "board/move.h"

namespace prodigy::movegen {
using MoveList = boost::container::static_vector<board::Move, 256>;
}
