cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)
set(FOLDER_NAME core)

set(SOURCES
    dataframe.h dataframe.cpp
    sph_interpolator.h sph_interpolator.cpp
    cosmic_web.h cosmic_web.cpp
)

list(APPEND TARGET_INCLUDE_DIRS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(${FOLDER_NAME} STATIC ${SOURCES})
add_library(vis21::${FOLDER_NAME} ALIAS ${FOLDER_NAME})
target_include_directories(${FOLDER_NAME} ${TARGET_INCLUDE_DIRS})
target_link_libraries(${FOLDER_NAME} #
    PUBLIC vis21::utils #
    ${VTK_LIBRARIES} #
    Eigen3::Eigen 
)

