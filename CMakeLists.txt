cmake_minimum_required(VERSION 3.0)

project(tests)
add_definitions(-Wall -ansi -pthread -std=c++11 -lncurses -Wall)
# set( CMAKE_CXX_FLAGS "-O0 -Wall -pedantic -ansi -std=c++11 --coverage" )
set(CMAKE_EXE_LINKER_FLAGS "-pthread -lncurses")


SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREAD ON)
find_package(PkgConfig)
PKG_CHECK_MODULES(GTKMM REQUIRED gtkmm-3.0)

FIND_PACKAGE(Boost 1.42.0 REQUIRED COMPONENTS system thread regex)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS} ./include)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS})
ADD_DEFINITIONS(${GTKMM_CFLAGS_OTHER})

SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

set(USED_SOURCE src/Point.cpp src/colider.cpp src/bot.cpp
  src/objects.cpp src/server.cpp src/snake.cpp)

add_executable(main src/main.cpp ${USED_SOURCE})
add_executable(client example_gtk.cpp  src/Point.cpp)
target_link_libraries(client ${USED_LIBS} ${GTKMM_LIBRARIES})
target_link_libraries(main ${USED_LIBS} ${GTKMM_LIBRARIES})
