set(PROJECT_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphAlgorithms.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphFunctions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NodeStorage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RandomGraphGenerator.cpp
)

set(PROJECT_CORE_HEADERS
    ${PROJECT_INCLUDE_DIR}/core/Graph.h
    ${PROJECT_INCLUDE_DIR}/core/GraphAlgorithms.h
    ${PROJECT_INCLUDE_DIR}/core/GraphFunctions.h
    ${PROJECT_INCLUDE_DIR}/core/DataGraph.h
    ${PROJECT_INCLUDE_DIR}/core/GraphTypes.h
    ${PROJECT_INCLUDE_DIR}/core/IGraph.h
    ${PROJECT_INCLUDE_DIR}/core/INode.h
    ${PROJECT_INCLUDE_DIR}/core/Node.h
    ${PROJECT_INCLUDE_DIR}/core/NodeStorage.h
    ${PROJECT_INCLUDE_DIR}/core/RandomGraphGenerator.h
    ${PROJECT_INCLUDE_DIR}/core/Utils.h
    ${PROJECT_INCLUDE_DIR}/core/Config.h
)

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

target_compile_features(${PROJECT_LIBRARY_TARGET} PUBLIC cxx_std_20)

target_compile_options(${PROJECT_LIBRARY_TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive /WX->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Werror>
)

target_link_libraries(${PROJECT_LIBRARY_TARGET} PUBLIC
    nlohmann_json::nlohmann_json
)
