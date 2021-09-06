#include "base/string_utils.h"

namespace prodigy {
std::vector<std::string_view> split(const std::string_view view, const std::string_view delims) {
  std::vector<std::string_view> tokens;
  split(
      view, [&](const auto token) { tokens.push_back(token); }, delims);
  return tokens;
}
}
