add_executable("${CMAKE_PROJECT_NAME}" prodigy.cpp)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE Boost::program_options uci)
install(TARGETS "${CMAKE_PROJECT_NAME}" DESTINATION bin)
