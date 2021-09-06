add_library(
  board

  bitboard.cpp
  castling_rights.cpp
  color.cpp
  coordinate.cpp
  file.cpp
  move.cpp
  piece_type.cpp
  position.cpp
  rank.cpp
)
target_link_libraries(board PRIVATE zobrist)

add_subdirectory(tests)
