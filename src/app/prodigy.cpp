#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <regex>

#include "base/ply.h"
#include "base/string_utils.h"
#include "board/position.h"
#include "movegen/move_generator.h"
#include "movegen/perft.h"
#include "search/random_searcher.h"
#include "uci/event_loop.h"

namespace prodigy {
namespace {
struct PerftParams final {
  board::Position position;
  Ply depth;
};

void validate(boost::any& out, const std::vector<std::string>& values, std::optional<PerftParams>*, int) {
  // NOTE: this regex produces false positives.
  static const std::regex FEN_REGEX(
      R"(([PNBRQKpnbrqk1-8]{1,8}\/){7}[PNBRQKpnbrqk1-8]{1,8} [wb] ([KQkq]{1,4}|-) ([a-h][36]|-) \d+ \d+)");

  const auto depth = values.size() == 2 ? to_arithmetic<Ply>(values.back()) : std::nullopt;
  if (!depth.has_value() || !std::regex_match(values.front().begin(), values.front().end(), FEN_REGEX)) {
    throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
  }

  out = std::optional(PerftParams{
      .position = board::Position::from_fen(values.front()),
      .depth = *depth,
  });
}
}
}

int main(int argc, char* argv[]) {
  std::optional<prodigy::PerftParams> perft_params;

  const auto command_line_options = [&] {
    boost::program_options::options_description command_line_options("OPTIONS");
    // clang-format off
    command_line_options.add_options()
        ("help,h", "Print help information.")
        ("perft", boost::program_options::value(&perft_params)->value_name("<FEN> <DEPTH>")->multitoken(), "Run perft.")
        ;
    // clang-format on
    return command_line_options;
  }();

  const auto variables_map = [&] {
    boost::program_options::variables_map variables_map;
    store(parse_command_line(argc, argv, command_line_options), variables_map);
    notify(variables_map);
    return variables_map;
  }();

  if (variables_map.contains("help")) {
    std::cout << command_line_options;
    return 0;
  }

  if (perft_params.has_value()) {
    const auto result = perft(prodigy::movegen::MoveGenerator(), perft_params->position, perft_params->depth);
    std::cout << '\n' << perft_params->position << '\n';
    std::cout << '\n' << result << '\n';
    return 0;
  }

  prodigy::uci::run_event_loop(std::make_unique<prodigy::search::RandomSearcher>());
}
