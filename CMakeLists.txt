cmake_minimum_required (VERSION 4.1.1)
project(psx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------
# Build speed: ccache (transformative on clean rebuilds / branch switches)
# ------------------------------------------------------------------
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# ------------------------------------------------------------------
# Build speed: faster linker (lld/mold). Link time dominates on big
# C++ projects pulling in Vulkan + Assimp + GLFW.
# ------------------------------------------------------------------
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC)
    find_program(LLD_LINK_PROGRAM lld-link)
    if(LLD_LINK_PROGRAM)
        add_link_options(-fuse-ld=lld-link)
    endif()
elseif(NOT MSVC)
    find_program(MOLD_PROGRAM mold)
    find_program(LLD_PROGRAM ld.lld)
    if(MOLD_PROGRAM)
        add_link_options(-fuse-ld=mold)
    elseif(LLD_PROGRAM)
        add_link_options(-fuse-ld=lld)
    endif()
endif()

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PRIVATE
    src/main.cpp

    src/cpu/cpu.cpp

    src/bios/bios.cpp
    src/ram/ram.cpp
    src/map/map.cpp
)
