#include "movegen/move_generator.h"
#include "search/searcher.h"

namespace prodigy::search {
class RandomSearcher final : public Searcher {
 private:
  std::optional<board::Move> search(const board::Position&) override;

  const movegen::MoveGenerator move_generator_;
};
}
