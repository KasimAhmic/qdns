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

add_library(libqdns
    src/core/byte_util.hpp
    src/core/dns.hpp
    src/core/header.hpp
    src/core/header.cpp
    src/core/error.hpp
)

target_include_directories(libqdns
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_compile_definitions(libqdns PRIVATE QDNS_BUILD_TYPE="$<CONFIG>")

add_library(qdns_sources 
    src/app_config.cpp
    src/app_config.hpp
    src/logging.cpp
    src/logging.hpp
    src/number_util.hpp
)

target_link_libraries(qdns_sources
    PUBLIC
        Boost::json
        spdlog::spdlog
        Boost::asio
        Boost::system
        Boost::interprocess
        Boost::program_options
)

add_executable(qdns src/main.cpp)

target_link_libraries(qdns PRIVATE libqdns qdns_sources)
target_include_directories(qdns PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_compile_definitions(qdns PRIVATE QDNS_BUILD_TYPE="$<CONFIG>")

include(CTest)

if (BUILD_TESTING)
    add_subdirectory(external/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)

    add_executable(qdns_tests
        test/test_main.cpp
        test/test_app_config.cpp
        test/test_logging.cpp
        test/core/test_header.cpp
        test/core/test_byte_util.cpp
    )

    target_link_libraries(qdns_tests
        PRIVATE
            libqdns
            qdns_sources
            GTest::gtest
    )

    include(GoogleTest)
    gtest_discover_tests(qdns_tests)
endif ()
