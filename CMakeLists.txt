cmake_minimum_required(VERSION 3.16)

# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------
project(ECT_SDK
    VERSION 1.0.0
    LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# Global C++ settings
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------
option(ECT_SDK_BUILD_EXAMPLES "Build ECT-SDK examples" ON)
option(ECT_SDK_BUILD_TESTS    "Build ECT-SDK tests"    ON)

# ------------------------------------------------------------------------------
# Library: ect_sdk
# ------------------------------------------------------------------------------
add_library(ect_sdk)

target_sources(ect_sdk
    PRIVATE
        src/ect_f_operator.cpp
        src/ect_e_operator.cpp
        src/ect_finv_operator.cpp
        src/ect_g_operator.cpp
        src/ect_sdk.cpp
)

target_include_directories(ect_sdk
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Warnings (strict but sane)
if (MSVC)
    target_compile_options(ect_sdk PRIVATE /W4)
else()
    target_compile_options(ect_sdk PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------
if (ECT_SDK_BUILD_EXAMPLES)
    add_executable(servo_position_control
        examples/servo_position_control.cpp
    )
    target_link_libraries(servo_position_control PRIVATE ect_sdk)

    add_executable(drone_altitude_control
        examples/drone_altitude_control.cpp
    )
    target_link_libraries(drone_altitude_control PRIVATE ect_sdk)
endif()

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
if (ECT_SDK_BUILD_TESTS)
    add_executable(simple_loop_test
        tests/simple_loop_test.cpp
    )
    target_link_libraries(simple_loop_test PRIVATE ect_sdk)
endif()
