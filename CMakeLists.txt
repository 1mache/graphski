cmake_minimum_required(VERSION 3.15...3.28)
project(graphski LANGUAGES CXX)

set(PROJECT_LIBRARY_TARGET "${PROJECT_NAME}_core")
set(PROJECT_GRAPHICS_TARGET "${PROJECT_NAME}_graphics")
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
# Fetch catch2 and include Ctest
include(CTest)
if(BUILD_TESTING)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v3.13.0
        GIT_SHALLOW TRUE
    ) 
    FetchContent_MakeAvailable(Catch2)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(Catch)
endif()

# 1. Handle Build Types for VS Code (Single-Config Generators)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(APP_ENTRYPOINT "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

option(STATIC_ANALYSIS "Enable static analysis tools (clang-tidy, cppcheck)" OFF)

# Analyzers:
if(STATIC_ANALYSIS)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
        # Run clang-tidy on all C++ files using checks from .clang-tidy.
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found!")
    endif()

    find_program(CPPCHECK_EXE NAMES "cppcheck")
    if(CPPCHECK_EXE)
        message(STATUS "Found cppcheck: ${CPPCHECK_EXE}")
        # Enable all checks, but suppress missing system includes (like <iostream>).
        set(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--suppress=missingIncludeSystem;--inconclusive")
    else()
        message(WARNING "cppcheck not found!")
    endif()
endif()

add_subdirectory(src/core)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(src/graphics)

add_executable(${PROJECT_APP_TARGET} ${APP_ENTRYPOINT})
target_link_libraries(${PROJECT_APP_TARGET} PRIVATE ${PROJECT_GRAPHICS_TARGET})

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

# Copies the "fonts" folder to the same directory as the compiled executable
add_custom_command(TARGET ${PROJECT_APP_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:${PROJECT_APP_TARGET}>/fonts"
)