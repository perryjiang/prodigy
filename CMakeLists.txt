cmake_minimum_required(VERSION 3.21.1)

project(prodigy VERSION 0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options unit_test_framework)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(BoostTestTarget)
include(CPack)
include(CodeCoverage)
include(Sanitizers)

enable_testing()

include_directories(src)
add_subdirectory(src)
