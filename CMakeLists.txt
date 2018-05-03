cmake_minimum_required (VERSION 2.6)
project (urf)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(/opt/ros/kinetic/include)
link_directories(/opt/ros/kinetic/lib)


#add_library(drv-filesystem SHARED "src/drv-filesystem.cpp")
#target_link_libraries(drv-filesystem )

#add_library(drv-yaml SHARED "src/drv-yaml.cpp")
#target_link_libraries(drv-yaml )


add_executable(teste "src/main4.cpp")
target_link_libraries(teste pthread yaml-cpp)
#target_link_libraries(teste roscpp rosconsole roscpp_serialization pthread)
