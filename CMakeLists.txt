cmake_minimum_required (VERSION 3.5)
if(NOT TARGET Serial)
project(Serial)
include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB SERIAL_SOURCES ${PROJECT_SOURCE_DIR}/src/serial_help*)
add_library(Serial SHARED ${SERIAL_SOURCES})
endif()

project (robot_controller)
SET(GCC_FLAGS "-g")
add_definitions(-DPOSIX -DXBEE_ATMODE_DEBUG -DXBEE_DEVICE_ENABLE_ATMODE ${GCC_FLAGS})
if(NOT TARGET XBee)
add_subdirectory (${PROJECT_SOURCE_DIR}/libs/xbee_ansic_library)
endif()
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  tf
)
catkin_package()
include_directories(
${PROJECT_SOURCE_DIR}/libs/xbee_ansic_library
${PROJECT_SOURCE_DIR}/libs/xbee_ansic_library/include
${catkin_INCLUDE_DIRS}
)
add_executable(robot_controller src/robot_controller.cpp)
target_link_libraries (robot_controller XBee Serial ${catkin_LIBRARIES})
