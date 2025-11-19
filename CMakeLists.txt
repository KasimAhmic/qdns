cmake_minimum_required(VERSION 3.31)
project(qdns LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost CONFIG REQUIRED COMPONENTS asio system)

add_executable(qdns src/main.cpp)

target_link_libraries(qdns
  PRIVATE
  Boost::asio
  Boost::system)

add_executable(nc src/client.cpp)

target_link_libraries(nc
        PRIVATE
        Boost::asio
        Boost::system)
