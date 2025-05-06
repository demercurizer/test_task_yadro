# tests/CMakeLists.txt
find_package(GTest REQUIRED)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../src
)

add_executable(runTests
    test_main.cpp
    ClubSystem_test.cpp
)

target_link_libraries(runTests
    PRIVATE
        GTest::GTest
        GTest::Main
        core
)

add_test(
    NAME ClubSystemTests
    COMMAND runTests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

