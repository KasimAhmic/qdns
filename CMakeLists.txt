cmake_minimum_required(VERSION 3.31)
project(qdns LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# spdlog configuration
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE_HO OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS_HO OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)

# Boost configuration
set(BOOST_INCLUDE_LIBRARIES asio system interprocess program_options json test CACHE STRING "" FORCE)

string(TIMESTAMP QDNS_BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S UTC" UTC)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/build_info.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/build_info.hpp"
    @ONLY
)

add_subdirectory(external/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(external/boost EXCLUDE_FROM_ALL)

add_executable(qdns src/main.cpp
        src/app_config.cpp
        src/app_config.hpp
        src/logging.cpp
        src/logging.hpp
)

target_link_libraries(qdns
        PRIVATE
        Boost::asio
        Boost::system
        Boost::interprocess
        Boost::program_options
        Boost::json
        spdlog::spdlog
)

target_include_directories(qdns
        PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_compile_definitions(qdns
        PRIVATE
        QDNS_BUILD_TYPE="$<CONFIG>"
)

include(CTest)

if (BUILD_TESTING)
    add_executable(qdns_tests test/test_main.cpp)

    target_link_libraries(qdns_tests
            PRIVATE
            Boost::unit_test_framework
    )

    add_test(
            NAME qdns.unit
            COMMAND qdns_tests
    )
endif ()
