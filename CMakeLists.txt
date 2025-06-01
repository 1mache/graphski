cmake_minimum_required(VERSION 3.15)
project(graphski LANGUAGES CXX)

# Let user select Debug or Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add src files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(graphski ${SOURCES})

# Include directories
target_include_directories(graphski PRIVATE
    "${CMAKE_SOURCE_DIR}/dependencies/SFML-3.0.0/include"
    "${CMAKE_SOURCE_DIR}/dependencies/nlohmann"
)

# Link SFML static libraries
target_link_directories(graphski PRIVATE "${CMAKE_SOURCE_DIR}/dependencies/SFML-3.0.0/lib")

target_compile_definitions(graphski PRIVATE SFML_STATIC)

# Link different libs based on config
target_link_libraries(graphski PRIVATE
    $<$<CONFIG:Debug>:sfml-system-s-d.lib sfml-graphics-s-d.lib sfml-window-s-d.lib freetyped.lib>
    $<$<CONFIG:Release>:sfml-system-s.lib sfml-graphics-s.lib sfml-window-s.lib freetype.lib>
    opengl32.lib gdi32.lib winmm.lib
)

# Output directory
set_target_properties(graphski PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/release"
)