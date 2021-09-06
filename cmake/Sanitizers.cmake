option(ASAN "Enable AddressSanitizer.")

if (ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

option(TSAN "Enable ThreadSanitizer.")

if (TSAN)
  add_compile_options(-fsanitize=thread)
  add_link_options(-fsanitize=thread)
endif()

option(UBSAN "Enable UndefinedBehaviorSanitizer.")

if (UBSAN)
  add_compile_definitions(PRODIGY_SANITIZE_UB)
  add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=undefined)
endif()
