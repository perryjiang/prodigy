#include "search/node_type.h"

#include <ostream>

namespace prodigy::search {
std::ostream& operator<<(std::ostream& os, const NodeType node_type) {
  switch (node_type) {
    case NodeType::PV:
      os << "PV";
      break;
    case NodeType::CUT:
      os << "CUT";
      break;
    case NodeType::ALL:
      os << "ALL";
      break;
  }
  return os;
}
}
