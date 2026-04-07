set(PROJECT_GRAPHICS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Arrow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DrawableGraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DrawableGraphFunctions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DrawableNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphEventHandler.cpp
)

set(PROJECT_GRAPHICS_HEADERS
    ${PROJECT_INCLUDE_DIR}/graphics/DrawableGraph.h
    ${PROJECT_INCLUDE_DIR}/graphics/DrawableGraphFunctions.h
    ${PROJECT_INCLUDE_DIR}/graphics/DrawableNode.h
    ${PROJECT_INCLUDE_DIR}/graphics/Arrow.h
    ${PROJECT_INCLUDE_DIR}/graphics/GraphEventHandler.h
    ${PROJECT_INCLUDE_DIR}/graphics/GraphicsConfig.h
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

target_compile_features(${PROJECT_GRAPHICS_TARGET} PUBLIC cxx_std_20)

target_compile_options(${PROJECT_GRAPHICS_TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive /WX->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Werror>
)

target_include_directories(${PROJECT_GRAPHICS_TARGET} PUBLIC
    "${PROJECT_INCLUDE_DIR}"
)

set(SFML_STATIC_LIBRARIES ON)
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/SFML-3.0.0/lib/cmake/SFML")
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

target_link_libraries(${PROJECT_GRAPHICS_TARGET} PUBLIC
    ${PROJECT_LIBRARY_TARGET}
    SFML::Graphics
    SFML::Window
    SFML::System
)
