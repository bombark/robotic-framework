cmake_minimum_required (VERSION 2.6)
project (urf)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(/opt/ros/kinetic/include)
link_directories(/opt/ros/kinetic/lib)


#add_library(drv-basic SHARED "src/drv-basic.cpp")
#target_link_libraries(drv-basic )


add_executable(teste "src/main4.cpp")
target_link_libraries(teste pthread)
#target_link_libraries(teste roscpp rosconsole roscpp_serialization pthread)
