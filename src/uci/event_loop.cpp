#include "uci/event_loop.h"

#include <boost/asio.hpp>
#include <utility>

#include "uci/stdin_handler.h"

namespace prodigy::uci {
void run_event_loop(std::unique_ptr<search::Searcher> searcher) {
  boost::asio::io_context io_context(1);
  const StdinHandler stdin_handler(io_context, std::move(searcher));
  io_context.run();
}
}
