cmake_minimum_required(VERSION 3.0)

project(tests)
add_definitions(-Wall -ansi -pthread -std=c++11 -lncurses -Wall)
# set( CMAKE_CXX_FLAGS "-O0 -Wall -pedantic -ansi -std=c++11 --coverage" )
set(CMAKE_EXE_LINKER_FLAGS "-pthread -lncurses")


SET(Boost_USE_STATIC_LIBS ON)
# SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.42.0 REQUIRED COMPONENTS system thread regex)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ./include)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

set(USED_SOURCE src/main.cpp src/Point.cpp src/acts.cpp src/bot.cpp
   src/client.cpp src/objects.cpp src/server.cpp src/snake.cpp)

add_executable(main ${USED_SOURCE})
target_link_libraries(main ncurses ${USED_LIBS})
