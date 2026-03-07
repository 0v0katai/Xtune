# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

set(XTUNE_VERSION "00.20.0000")
cmake_minimum_required(VERSION 3.15)
project(Xtune LANGUAGES ASM C VERSION ${XTUNE_VERSION})

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH3)
include(GitVersionNumber)
include(Fxconv)
find_package(Gint 2.11 REQUIRED)
find_package(cBench 1.0 REQUIRED)
find_package(Azur 0.1)
find_package(Git REQUIRED)

git_version_number(GIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.git"
  SHORT_HASH "XTUNE_GIT_HASH"
  TAG_RELATIVE "XTUNE_GIT_VERSION")

set(SOURCES
  src/main.c
  src/data.c
  src/help.c
  src/validate.c
  src/util.c
  src/syscall.s
  src/menu/express.c
  src/menu/settings.c
  src/menu/bsc.c
  src/menu/mem_data.c
  src/benchmark/mem_bench.s
  src/benchmark/tick_count.s
  src/benchmark/benchmark.c
  src/memtest/rom_test.c
  src/memtest/sram_test.c
  src/memtest/sdram_test.c
  src/memtest/test_address.c
  src/memtest/mem_read.s
  src/memtest/mem_write.s
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
  # ...
)
set(ASSETS_fx
  # ...
)
set(ASSETS_cg
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

configure_file(config.h.in include/config.h)
add_executable(Xtune ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_include_directories(Xtune PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_options(Xtune PRIVATE -Wall -Wextra -Os -g -flto=auto)
target_link_options(Xtune PRIVATE -Wl,-Map=map -Wl,-print-memory-usage -Wl,-gc-sections)
target_link_libraries(Xtune Gint::Gint cBench::cBench #[[Azur::Azur]])

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET Xtune VERSION ${Xtune_VERSION} OUTPUT "Ftune4.g1a"
    NAME "Ftune4" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET Xtune VERSION ${Xtune_VERSION} OUTPUT "Ptune4.g3a"
    NAME "Ptune4" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET Xtune VERSION ${Xtune_VERSION} OUTPUT "Ptune4-fx.g3a"
    NAME "Ptune4-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh3(TARGET Xtune OUTPUT "CPtune4.hh3")
endif()
