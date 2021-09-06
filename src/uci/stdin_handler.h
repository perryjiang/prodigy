#pragma once

#include <boost/asio.hpp>
#include <cstddef>
#include <memory>

#include "search/searcher.h"
#include "uci/handler.h"

namespace prodigy::uci {
class StdinHandler final {
 public:
  StdinHandler(boost::asio::io_context&, std::unique_ptr<search::Searcher>);

  StdinHandler(const StdinHandler&) = delete;
  StdinHandler& operator=(const StdinHandler&) = delete;

 private:
  void async_read_line();
  void handle_read_line(const boost::system::error_code&, std::size_t bytes_transferred);

  boost::asio::posix::stream_descriptor input_;
  boost::asio::streambuf streambuf_;
  Handler uci_handler_;
};
}
