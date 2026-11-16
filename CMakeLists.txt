cmake_minimum_required(VERSION 3.16)
project(Prac15 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(prac15_config src/config.cpp src/output.cpp)
target_include_directories(prac15_config PUBLIC include)

add_library(prac15_toro src/toro_cases.cpp)
target_include_directories(prac15_toro PUBLIC include)

add_library(prac15_riemann
    src/wave_function.cpp
    src/pressure_solver.cpp
    src/star_state.cpp
    src/wave_speeds.cpp
    src/sample_solution.cpp
    src/face_flux.cpp
    src/vacuum.cpp
    src/exact_riemann_solver.cpp
    src/exact_flux_solver.cpp
    src/solver_factory.cpp
)
target_include_directories(prac15_riemann PUBLIC include)

add_executable(prac15 main.cpp)
target_link_libraries(prac15 PRIVATE prac15_config prac15_riemann)

enable_testing()
add_executable(toro_tests tests/toro_tests.cpp)
target_link_libraries(toro_tests PRIVATE prac15_toro prac15_config prac15_riemann)
add_test(NAME toro_tests COMMAND toro_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(prac15_config PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(prac15_riemann PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(prac15_toro PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(prac15 PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(toro_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()
