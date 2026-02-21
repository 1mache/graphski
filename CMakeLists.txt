cmake_minimum_required(VERSION 3.15...3.28)
project(graphski LANGUAGES CXX)

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

add_executable(graphski ${SOURCES})

# 3. Modern C++ Standard definition (Target-centric)
target_compile_features(graphski PRIVATE cxx_std_17)

# Set compiler warnings (pedantic/permissive- intentionally omitted)
target_compile_options(graphski PRIVATE
    # If using MSVC compiler:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    
    # If using GCC or Clang :
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic>
)

# 4. Include and Link Directories
# Using CMAKE_CURRENT_SOURCE_DIR is safer if this ever becomes a subproject
target_include_directories(graphski PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
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
) 

# 6. Output Directories
set_target_properties(graphski PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/release"
)