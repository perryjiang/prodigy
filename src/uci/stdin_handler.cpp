#include "uci/stdin_handler.h"

#include <unistd.h>

#include <iostream>
#include <string>
#include <utility>

namespace prodigy::uci {
StdinHandler::StdinHandler(boost::asio::io_context& io_context, std::unique_ptr<search::Searcher> searcher)
    : input_(io_context, ::dup(STDIN_FILENO)), uci_handler_(std::cout, std::move(searcher)) {
  async_read_line();
}

void StdinHandler::async_read_line() {
  boost::asio::async_read_until(input_, streambuf_, '\n', [&](const auto& error_code, const auto bytes_transferred) {
    handle_read_line(error_code, bytes_transferred);
  });
}

void StdinHandler::handle_read_line(const boost::system::error_code& error_code, const std::size_t bytes_transferred) {
  if (error_code) {
    std::clog << __func__ << ": " << error_code << '\n';
    return;
  }
  const std::string message(
      boost::asio::buffers_begin(streambuf_.data()),
      boost::asio::buffers_begin(streambuf_.data()) + static_cast<std::ptrdiff_t>(bytes_transferred));
  streambuf_.consume(bytes_transferred);
  if (uci_handler_.handle(message)) {
    async_read_line();
  }
}
}
