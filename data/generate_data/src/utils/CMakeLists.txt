cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)

set(FOLDER_NAME utils)

set(SOURCES
    nanoflann.hpp
    interp_utils.h
    kd_tree.h kd_tree.cpp
)

list(APPEND TARGET_INCLUDE_DIRS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(${FOLDER_NAME} STATIC ${SOURCES})
add_library(vis21::${FOLDER_NAME} ALIAS ${FOLDER_NAME})
target_include_directories(${FOLDER_NAME} ${TARGET_INCLUDE_DIRS})
target_link_libraries(${FOLDER_NAME} Eigen3::Eigen)

