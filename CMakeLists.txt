cmake_minimum_required(VERSION 3.10)
project(DungeonEditor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/Knight.cpp
    src/Druid.cpp
    src/Elf.cpp
)

add_executable(dungeon_editor ${SOURCES})


target_include_directories(dungeon_editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()
    
    add_executable(dungeon_tests
        tests/test_main.cpp
        src/Knight.cpp
        src/Druid.cpp
        src/Elf.cpp
    )
    
    target_link_libraries(dungeon_tests
        GTest::gtest
        GTest::gtest_main
    )
    
    target_include_directories(dungeon_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    
    include(GoogleTest)
    gtest_discover_tests(dungeon_tests)
    
    message(STATUS "Tests enabled. Build target: dungeon_tests")
endif()
