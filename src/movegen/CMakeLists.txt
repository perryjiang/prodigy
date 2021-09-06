add_library(movegen magic_bitboards.cpp move_generator.cpp perft.cpp tables.cpp)
target_link_libraries(movegen INTERFACE board)

add_subdirectory(tests)
