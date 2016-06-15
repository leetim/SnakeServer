cmake_minimum_required(VERSION 3.0)

project(asioExample)
add_definitions(-Wall -ansi -pthread -std=c++11)
set(CMAKE_EXE_LINKER_FLAGS "-pthread")

SET(Boost_USE_STATIC_LIBS ON)
# SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.42.0 REQUIRED COMPONENTS system thread regex)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

# include_directories(${Boost_INCLUDE_DIR})

add_executable(client client.cpp)
target_link_libraries(client ${USED_LIBS})

add_executable(server server.cpp)
target_link_libraries(server ${USED_LIBS})
