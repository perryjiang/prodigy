add_library(search controller.cpp node_type.cpp random_searcher.cpp)
target_link_libraries(search PRIVATE Threads::Threads movegen)

add_subdirectory(tests)
