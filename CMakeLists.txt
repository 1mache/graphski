cmake_minimum_required(VERSION 3.15...3.28)
project(graphski LANGUAGES CXX)

set(PROJECT_LIBRARY_TARGET "${PROJECT_NAME}_core")
set(PROJECT_APP_TARGET "${PROJECT_NAME}_app")
set(PROJECT_APP_OUTPUT_NAME "${PROJECT_NAME}")

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
# Note: While explicit listing is safest, GLOB with CONFIGURE_DEPENDS is fine for this setup
set(APP_ENTRYPOINT "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

file(GLOB_RECURSE PROJECT_CPP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

set(PROJECT_LIBRARY_SOURCES ${PROJECT_CPP_SOURCES})
list(REMOVE_ITEM PROJECT_LIBRARY_SOURCES "${APP_ENTRYPOINT}")

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

add_library(${PROJECT_LIBRARY_TARGET} ${PROJECT_LIBRARY_SOURCES} ${PROJECT_HEADERS})
add_executable(${PROJECT_APP_TARGET} ${APP_ENTRYPOINT})

# 3. Modern C++ Standard definition (Target-centric)
target_compile_features(${PROJECT_LIBRARY_TARGET} PUBLIC cxx_std_20)

# Set compiler warnings (pedantic/permissive- intentionally omitted)
target_compile_options(${PROJECT_LIBRARY_TARGET} PRIVATE
    # If using MSVC compiler:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive /WX->
    
    # If using GCC or Clang :
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Werror>
)

# 4. Include and Link Directories
# Using CMAKE_CURRENT_SOURCE_DIR is safer if this ever becomes a subproject
target_link_libraries(${PROJECT_APP_TARGET} PRIVATE ${PROJECT_LIBRARY_TARGET})

target_include_directories(${PROJECT_LIBRARY_TARGET} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
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
target_link_libraries(${PROJECT_LIBRARY_TARGET} PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
    nlohmann_json::nlohmann_json
) 

# Suppress the missing PDB warnings from third-party static libraries
target_link_options(${PROJECT_APP_TARGET} PRIVATE 
    "LINKER:/ignore:4099"
)

# 6. Output Directories
set_target_properties(${PROJECT_APP_TARGET} PROPERTIES
    OUTPUT_NAME "${PROJECT_APP_OUTPUT_NAME}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/release"
)

enable_testing()
add_subdirectory(tests)

# Copies the "fonts" folder to the same directory as the compiled executable
add_custom_command(TARGET ${PROJECT_APP_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:${PROJECT_APP_TARGET}>/fonts"
)