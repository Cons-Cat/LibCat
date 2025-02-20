cmake_minimum_required(VERSION 3.24)

project(cat LANGUAGES CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  option(CAT_USE_SANITIZERS "Link ASan and UBSan. This doesn't work in Clang." OFF)
else()
  option(CAT_USE_SANITIZERS "Link ASan and UBSan" ON)
endif()

list(
  APPEND CAT_COMPILE_OPTIONS_COMMON
  # Disable linking libC symbols.
  -nostdlib
  -nostdlib++
  # These features are pessimizations to libCat:
  # TODO: Exceptions and rtti should have optional support once it's possible to
  # implement them.
  -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables
  # `global_includes.hpp` must be available everywhere.
  -include global_includes.hpp

  # Enable CPU intrinsics:
  -msse4.2
  -mavx2
  -mfma
  -mlzcnt
  -mbmi
  -mbmi2
  -mfsgsbase
  
  # Enable most warnings.
  -Wall -Wextra
  -Wno-unused-function
  -Wno-unknown-pragmas
  -Wno-missing-braces # This breaks `cat::tuple`.
  
  # Both GCC and Clang attributes are used.
  # See p2565 "Supporting User-Defined Attributes"
  $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>
  $<$<CXX_COMPILER_ID:Clang>:-Wno-unknown-attributes>
  # Disable some Clang warnings. libCat needs to violate these.
  $<$<CXX_COMPILER_ID:Clang>:-Wno-unqualified-std-cast-call>
  $<$<CXX_COMPILER_ID:Clang>:-Wno-main>
  # This produces many false positives in Clang 17.
  $<$<CXX_COMPILER_ID:Clang>:-Wno-redundant-consteval-if>
  # Use GCC extensions.
  $<$<CXX_COMPILER_ID:Clang>:-Wno-gnu>

  # Use Clang lifetime analysis.
  $<$<CXX_COMPILER_ID:Clang>:-Wdangling -Wdangling-gsl -Xclang -fexperimental-bounds-safety>
)

if(CAT_USE_SANITIZERS)
    list(
     APPEND CAT_COMPILE_OPTIONS_COMMON
    -fomit-frame-pointer # Required for UBsan.
    -fsanitize=undefined
    -fsanitize=address

    #$<$<CXX_COMPILER_ID:GNU>:--param asan-stack=0>
    #$<$<CXX_COMPILER_ID:Clang>:-mllvm -asan-stack=0>
  )
endif()

list(
  APPEND CAT_COMPILE_OPTIONS_DEBUG
  -ggdb3
  $<$<CXX_COMPILER_ID:GNU>:-fno-eliminate-unused-debug-types
                           -fno-eliminate-unused-debug-symbols>
  $<$<CXX_COMPILER_ID:Clang>:-gmodules -gfull>
)

list(
  APPEND CAT_COMPILE_OPTIONS_RELEASE
  # Remove unused symbols.
  -ffunction-sections -fdata-sections
  -fvisibility=hidden -fvisibility-inlines-hidden
  # TODO: Consider why no-plt instead of relro.
  -fno-plt
)

list(
  APPEND CAT_COMPILE_OPTIONS_RELWITHDEBINFO
  ${CAT_COMPILE_OPTIONS_DEBUG}
  ${CAT_COMPILE_OPTIONS_RELEASE}
)

# LTO doesn't work with sanitizers in clang, so it's special-cased.
if (NOT CAT_USE_SANITIZERS)
  list(
    APPEND CAT_COMPILE_OPTIONS_RELEASE
    -flto=auto
  )

  list(
    APPEND CAT_COMPILE_OPTIONS_RELWITHDEBINFO
    # Use thin LTO for optimized non-release builds.
    -flto=thin
  )
endif()

list(
  APPEND CAT_COMPILE_OPTIONS
  ${CAT_COMPILE_OPTIONS_COMMON}
  $<$<CONFIG:Debug>:${CAT_COMPILE_OPTIONS_DEBUG}>
  $<$<CONFIG:Release>:${CAT_COMPILE_OPTIONS_RELEASE}>
  $<$<CONFIG:RelWithDebInfo>:${CAT_COMPILE_OPTIONS_RELWITHDEBINFO}>
)

list(
  APPEND CAT_LINK_OPTIONS_COMMON
  # This is required to prevent duplicate symbols.
  -nostdlib  
  # `lld` is required for LLVM LTO. Bfd and `mold` do not work.
  $<$<CXX_COMPILER_ID:Clang>:-fuse-ld=lld>
)

list(
  APPEND CAT_LINK_OPTIONS_RELEASE
  # Remove unused symbols.
  -Wl,-z,noseparate-code,--gc-sections
)

list(
  APPEND CAT_LINK_OPTIONS
  ${CAT_LINK_OPTIONS_COMMON}
  $<$<CONFIG:Release>:${CAT_LINK_OPTIONS_RELEASE}>
  $<$<CONFIG:RelWithDebInfo>:${CAT_LINK_OPTIONS_RELEASE}>
)

add_library(cat INTERFACE)

# Everything building with `cat` should be at least C++26, with GNU extensions.

# This is broken for Clang 19 until CMake 3.29.2
# target_compile_features(cat INTERFACE cxx_std_26)
target_compile_options(cat INTERFACE "-std=gnu++26")

# Link the address and undefined behavior sanitizers.
if (CAT_USE_SANITIZERS)
  target_link_options(cat INTERFACE
    -fsanitize=address
    #-fsanitize=undefined
    $<$<CXX_COMPILER_ID:Clang>:-shared-libasan>
  )
endif()

target_compile_options(cat INTERFACE ${CAT_COMPILE_OPTIONS})
target_link_options(cat INTERFACE ${CAT_LINK_OPTIONS})

# TODO: Implement these symbols in libCat directly.
# target_link_libraries(cat INTERFACE gcc)

# `src/CMakeLists.txt` adds all of the source and headers files to `cat`.
add_subdirectory(src/)

# Tests can be run using ctest, or by manually running the `unit_tests`
# executable from your specified build output directory.
enable_testing()
# Build the tests.
add_subdirectory(tests/)

# Build the examples.
add_subdirectory(examples/)

# # `ctor.ld` is a minimal linker script necessary for thread local storage.
# target_link_options(cat INTERFACE -T ${CMAKE_SOURCE_DIR}/src/ctor.ld)
# set_target_properties(
#   cat PROPERTIES
#   LINK_DEPENDS ${CMAKE_SOURCE_DIR}/src/ctor.ld
# )

# Find a configurable path to a `clang-format` binary.
find_program(CAT_CLANG_FORMAT_PATH "clang-format" DOC "`clang-format` binary to use.")
get_property(CAT_SOURCES TARGET cat PROPERTY INTERFACE_SOURCES)
# Recursively discover the `.hpp` files.
get_property(CAT_HEADERS TARGET cat PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE CAT_HEADERS "*.hpp")

option(CAT_PCH "Enable precompiled-headers for libCat headers." ON)
if (CAT_PCH)
  target_precompile_headers(cat INTERFACE ${CAT_HEADER_FILES})  
endif()
set_target_properties(cat PROPERTIES PCH_WARN_INVALID ON)

# Run `clang-format` on all source files of the `cat` target.
add_custom_target(cat-format
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/format.cmake ${CAT_CLANG_FORMAT_PATH} \"${CAT_SOURCES} ${CAT_HEADERS} ${CAT_HEADER_FILES} \"
  DEPENDS ${CAT_SOURCES} ${CAT_HEADERS}
)
