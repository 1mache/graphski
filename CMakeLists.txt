cmake_minimum_required(VERSION 3.15...3.28)
project(graphski LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json DESTINATION ${CMAKE_SOURCE_DIR})

# Cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(AddGitSubmodule)
include(FetchContent)
# Fetch nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3 
    GIT_SHALLOW TRUE
) 
FetchContent_MakeAvailable(nlohmann_json)  
# Fetch catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v3.13.0
    GIT_SHALLOW TRUE
) 
FetchContent_MakeAvailable(Catch2)

# 1. Handle Build Types for VS Code (Single-Config Generators)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# 2. Add source files
# Note: While explicit listing is safest, GLOB with CONFIGURE_DEPENDS is fine for a single-file setup
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

option(STATIC_ANALYSIS "Enable static analysis tools (clang-tidy, cppcheck)" OFF)

# Analyzers:
if(STATIC_ANALYSIS)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
        # Run clang-tidy on all C++ files using checks from .clang-tidy.
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found! Is LLVM in your PATH?")
    endif()

    # ==========================================
    # 2. CPPCHECK (The Deep Static Analyzer)
    # ==========================================
    find_program(CPPCHECK_EXE NAMES "cppcheck")
    if(CPPCHECK_EXE)
        message(STATUS "Found cppcheck: ${CPPCHECK_EXE}")
        # Run cppcheck. Enable all checks, but suppress missing system includes (like <iostream>).
        set(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--suppress=missingIncludeSystem;--inconclusive")
    else()
        message(WARNING "cppcheck not found! Please install it and add to PATH.")
    endif()
endif()

add_executable(graphski ${SOURCES})

# 3. Modern C++ Standard definition (Target-centric)
target_compile_features(graphski PRIVATE cxx_std_20)

# Set compiler warnings (pedantic/permissive- intentionally omitted)
target_compile_options(graphski PRIVATE
    # If using MSVC compiler:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive /WX->
    
    # If using GCC or Clang :
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Werror>
)

# 4. Include and Link Directories
# Using CMAKE_CURRENT_SOURCE_DIR is safer if this ever becomes a subproject
target_include_directories(graphski PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(graphski SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/nlohmann"
)

# 5. Link SFML libraries
# 5.1. Tell CMake we want static libraries
set(SFML_STATIC_LIBRARIES ON)

# 5.2. Point CMake to the folder containing SFMLConfig.cmake
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SFML-3.0.0/lib/cmake/SFML")

# 5.3. Find SFML 3 (Note: SFML 3 requires capitalized component names)
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# 5.4. Link using modern namespaces!
# This automatically handles includes, library order, transitive dependencies (opengl32, etc.), and sets -DSFML_STATIC!
target_link_libraries(graphski PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    nlohmann_json::nlohmann_json
) 

target_link_libraries(graphski PRIVATE
    nlohmann_json::nlohmann_json
    Catch2::Catch2
)

# Suppress the missing PDB warnings from third-party static libraries
target_link_options(${PROJECT_NAME} PRIVATE 
    "LINKER:/ignore:4099"
)

# 6. Output Directories
set_target_properties(graphski PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/release"
)