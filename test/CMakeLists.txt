# Host (PC) unit tests for the target independent parts of PrintfTask.
#
# Uses the Unity test framework that ships with ESP-IDF, so IDF_PATH must be set (or pass -DUNITY_DIR=<path
# to Unity's src directory>). Requires a host C and C++ compiler (e.g. gcc/g++ from MinGW).
#
# Build and run:
#   cmake -S test -B test/build -G Ninja
#   cmake --build test/build
#   ctest --test-dir test/build --output-on-failure
cmake_minimum_required(VERSION 3.22)
project(PrintfTaskTests C CXX)

# Unity is C, the project code and tests are C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Lets VS Code (C/C++ extension) resolve includes in the test files
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(UNITY_DIR "$ENV{IDF_PATH}/components/unity/unity/src" CACHE PATH "Path to the Unity test framework src directory")
if(NOT EXISTS "${UNITY_DIR}/unity.c")
    message(FATAL_ERROR "Unity not found at '${UNITY_DIR}'. Set IDF_PATH or pass -DUNITY_DIR=<path to Unity src>.")
endif()

add_library(unity STATIC ${UNITY_DIR}/unity.c)
target_include_directories(unity PUBLIC ${UNITY_DIR})

set(MAIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../main)

add_executable(test_PrintBuffer
    test_PrintBuffer.cpp
)
target_include_directories(test_PrintBuffer PRIVATE ${MAIN_DIR}/Inc)
target_link_libraries(test_PrintBuffer PRIVATE unity)

# PrintUtils (and the PrintBuffer it owns) with a fake StdOutPort (defined in the test), so no target code is needed
add_executable(test_PrintUtils
    test_PrintUtils.cpp
    ${MAIN_DIR}/Src/PrintUtils.cpp
)
target_include_directories(test_PrintUtils PRIVATE ${MAIN_DIR}/Inc)
target_link_libraries(test_PrintUtils PRIVATE unity)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_PrintBuffer PRIVATE -Wall -Wextra -Werror)
    target_compile_options(test_PrintUtils PRIVATE -Wall -Wextra -Werror)
endif()

enable_testing()
add_test(NAME PrintBuffer COMMAND test_PrintBuffer)
add_test(NAME PrintUtils COMMAND test_PrintUtils)
