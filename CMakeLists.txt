cmake_minimum_required(VERSION 3.20)
project(TextAnalyzerLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Бібліотека
add_library(TextAnalyzerLib STATIC
    src/TextAnalyzer.cpp
)
target_include_directories(TextAnalyzerLib PUBLIC include)

# 2. Демонстрація 1
add_executable(main_demo
    apps/main_demo.cpp
)
target_link_libraries(main_demo PRIVATE TextAnalyzerLib)

# 3. Демонстрація 2 (некоректні винятки)
add_executable(bad_exceptions
    apps/bad_exceptions.cpp
)

# 4. Тести GTest
find_package(GTest CONFIG REQUIRED)

enable_testing()
add_executable(unit_tests
    tests/tests.cpp
)
target_link_libraries(unit_tests PRIVATE TextAnalyzerLib GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)