add_library(uci event_loop.cpp handler.cpp messages.cpp stdin_handler.cpp)
target_link_libraries(uci PRIVATE base board search)

add_subdirectory(tests)
