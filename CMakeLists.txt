cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX+FLAGS}")
set(CMAKE_CXX_COMPILER "g++-4.9")

project("seco")

set(seco_VERSION_MAJOR 1)
set(seco_VERSION_MINOR 0)

configure_file(
	"${PROJECT_SOURCE_DIR}/SecoConfig.h.in"
	"${PROJECT_BINARY_DIR}/SecoConfig.h"
)

include_directories("${PROJECT_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/core")
add_subdirectory(core)

add_executable(SerialCommunicator main.cxx)
target_link_libraries(SerialCommunicator core)
