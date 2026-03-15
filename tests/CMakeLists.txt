set(TEST_MAIN "unit_tests")
set(TEST_SOURCES 
    "NodeTests.cpp"
    "EdgeTests.cpp"
    "GraphTests.cpp"
)
set(TEST_INCLUDES "./") 

add_executable(${TEST_MAIN} ${TEST_SOURCES})
target_include_directories(${TEST_MAIN} PRIVATE ${TEST_INCLUDES})

target_link_libraries(${TEST_MAIN} PRIVATE ${PROJECT_LIBRARY_TARGET} Catch2::Catch2WithMain)

catch_discover_tests(${TEST_MAIN})