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

    add_executable(saturation_stress_loop
    examples/saturation_stress_loop.cpp
)
target_link_libraries(saturation_stress_loop PRIVATE ect_sdk)

add_executable(noise_injection_loop
    examples/noise_injection_loop.cpp
)
target_link_libraries(noise_injection_loop PRIVATE ect_sdk)

add_executable(time_varying_target
    examples/time_varying_target.cpp
)
target_link_libraries(time_varying_target PRIVATE ect_sdk)

add_executable(time_varying_target_saturation_limit
    examples/time_varying_target_saturation_limit.cpp
)
target_link_libraries(time_varying_target_saturation_limit PRIVATE ect_sdk)

endif()

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
if (ECT_SDK_BUILD_TESTS)
    add_executable(simple_loop_test
        tests/simple_loop_test.cpp
    )
    target_link_libraries(simple_loop_test PRIVATE ect_sdk)

    add_executable(determinism_test
        tests/determinism_test.cpp
    )
    target_link_libraries(determinism_test PRIVATE ect_sdk)

    add_executable(boundedness_test
    tests/boundedness_test.cpp
)
target_link_libraries(boundedness_test PRIVATE ect_sdk)

add_executable(monotonicity_test
    tests/monotonicity_test.cpp
)
target_link_libraries(monotonicity_test PRIVATE ect_sdk)

add_executable(sign_preservation_test
    tests/sign_preservation_test.cpp
)
target_link_libraries(sign_preservation_test PRIVATE ect_sdk)

add_executable(contraction_ratio_test
    tests/contraction_ratio_test.cpp
)
target_link_libraries(contraction_ratio_test PRIVATE ect_sdk)

endif()

