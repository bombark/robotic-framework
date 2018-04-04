cmake_minimum_required (VERSION 2.6)
project (urf)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)



add_executable(teste "src/main.cpp")
target_link_libraries(teste yaml-cpp)
