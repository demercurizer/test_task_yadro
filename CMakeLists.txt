# CMakeLists.txt (root)
cmake_minimum_required(VERSION 3.10)
project(ComputerClubSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

add_library(core
    src/ClubSystem.cpp
)

add_executable(task
    src/main.cpp
)

target_link_libraries(task
    PRIVATE core
)

file(
    COPY ${PROJECT_SOURCE_DIR}/tests/fixtures
    DESTINATION ${PROJECT_BINARY_DIR}/tests
)

enable_testing()
add_subdirectory(tests)

