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
    add_subdirectory(tests)
endif()

# 1. Handle Build Types for VS Code (Single-Config Generators)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(APP_ENTRYPOINT "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(PROJECT_CORE_SOURCES
    src/core/Graph.cpp
    src/core/GraphAlgorithms.cpp
    src/core/GraphFunctions.cpp
    src/core/NodeStorage.cpp
    src/core/RandomGraphGenerator.cpp
)

set(PROJECT_CORE_HEADERS
    include/core/Graph.h
    include/core/GraphAlgorithms.h
    include/core/GraphFunctions.h
    include/core/DataGraph.h
    include/core/GraphTypes.h
    include/core/IGraph.h
    include/core/INode.h
    include/core/Node.h
    include/core/NodeStorage.h
    include/core/RandomGraphGenerator.h
    include/core/Utils.h
    include/core/Config.h
)

set(PROJECT_GRAPHICS_SOURCES
    src/graphics/Arrow.cpp
    src/graphics/DrawableGraph.cpp
    src/graphics/DrawableGraphFunctions.cpp
    src/graphics/DrawableNode.cpp
    src/graphics/GraphEventHandler.cpp
)

set(PROJECT_GRAPHICS_HEADERS
    include/graphics/DrawableGraph.h
    include/graphics/DrawableGraphFunctions.h
    include/graphics/DrawableNode.h
    include/graphics/Arrow.h
    include/graphics/GraphEventHandler.h
    include/graphics/GraphicsConfig.h
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

add_library(${PROJECT_LIBRARY_TARGET})
target_sources(${PROJECT_LIBRARY_TARGET}
    PRIVATE
        ${PROJECT_CORE_SOURCES}
    PUBLIC
        FILE_SET coreHeaders
        TYPE HEADERS
        BASE_DIRS
            ${PROJECT_INCLUDE_DIR}
        FILES
            ${PROJECT_CORE_HEADERS}
)

add_library(${PROJECT_GRAPHICS_TARGET})
target_sources(${PROJECT_GRAPHICS_TARGET}
    PRIVATE
        ${PROJECT_GRAPHICS_SOURCES}
    PUBLIC
        FILE_SET graphicsHeaders
        TYPE HEADERS
        BASE_DIRS
            ${PROJECT_INCLUDE_DIR}
        FILES
            ${PROJECT_GRAPHICS_HEADERS}
)

add_executable(${PROJECT_APP_TARGET} ${APP_ENTRYPOINT})

# 3. Modern C++ Standard definition (Target-centric)
target_compile_features(${PROJECT_LIBRARY_TARGET} PUBLIC cxx_std_20)
target_compile_features(${PROJECT_GRAPHICS_TARGET} PUBLIC cxx_std_20)

# Set compiler warnings (pedantic/permissive- intentionally omitted)
target_compile_options(${PROJECT_LIBRARY_TARGET} PRIVATE
    # If using MSVC compiler:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive /WX->
    
    # If using GCC or Clang :
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Werror>
)

target_compile_options(${PROJECT_GRAPHICS_TARGET} PRIVATE
    # If using MSVC compiler:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive /WX->

    # If using GCC or Clang :
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Werror>
)

# 4. Include and Link Directories
# Public include paths stay rooted at include/ so callers must use core/... and graphics/...
target_include_directories(${PROJECT_LIBRARY_TARGET} PUBLIC
    "${PROJECT_INCLUDE_DIR}"
)

target_include_directories(${PROJECT_GRAPHICS_TARGET} PUBLIC
    "${PROJECT_INCLUDE_DIR}"
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
    nlohmann_json::nlohmann_json
)

target_link_libraries(${PROJECT_GRAPHICS_TARGET} PUBLIC
    ${PROJECT_LIBRARY_TARGET}
    SFML::Graphics
    SFML::Window
    SFML::System
) 

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