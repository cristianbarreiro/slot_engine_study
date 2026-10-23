cmake_minimum_required(VERSION 3.16)
project(SlotEngineStudy VERSION 1.0.0 LANGUAGES CXX)

option(SLOT_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(slot_apply_options target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
        if(SLOT_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
endfunction()

add_library(slot_engine
    src/evaluator.cpp
    src/game_engine.cpp
    src/simulator.cpp
)
target_include_directories(slot_engine PUBLIC include)
slot_apply_options(slot_engine)

add_executable(slot_simulator src/main.cpp)
target_link_libraries(slot_simulator PRIVATE slot_engine)
slot_apply_options(slot_simulator)

add_executable(slot_engine_tests tests/test_engine.cpp)
target_link_libraries(slot_engine_tests PRIVATE slot_engine)
slot_apply_options(slot_engine_tests)

enable_testing()
add_test(NAME slot_engine_tests COMMAND slot_engine_tests)
