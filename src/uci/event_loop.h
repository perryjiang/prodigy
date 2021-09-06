#pragma once

#include <memory>

#include "search/searcher.h"

namespace prodigy::uci {
void run_event_loop(std::unique_ptr<search::Searcher>);
}
