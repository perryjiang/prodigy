add_library(transposition_table bucket.cpp entry.cpp transposition_table.cpp value.cpp)
target_link_libraries(transposition_table INTERFACE search)

add_subdirectory(tests)
