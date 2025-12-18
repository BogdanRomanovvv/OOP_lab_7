cmake_minimum_required(VERSION 3.10)
project(DungeonEditor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем поддержку потоков
find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Исходники для оригинальной версии (Лаб 6)
set(SOURCES_LAB6
    src/main.cpp
    src/Knight.cpp
    src/Druid.cpp
    src/Elf.cpp
    src/Observer.cpp
)

# Исходники для асинхронной версии (Лаб 7)
set(SOURCES_LAB7
    src/main_async.cpp
    src/Knight.cpp
    src/Druid.cpp
    src/Elf.cpp
    src/Observer.cpp
)

# Сборка оригинальной версии (Лаб 6)
add_executable(dungeon_editor ${SOURCES_LAB6})
target_include_directories(dungeon_editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Сборка асинхронной версии (Лаб 7)
add_executable(dungeon_async ${SOURCES_LAB7})
target_include_directories(dungeon_async PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(dungeon_async PRIVATE Threads::Threads)

# Опция для тестов
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
        src/Observer.cpp
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
