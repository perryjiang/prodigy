option(CODE_COVERAGE "Enable code coverage.")

if (CODE_COVERAGE)
  add_compile_options(--coverage)
  add_link_options(--coverage)
endif()
