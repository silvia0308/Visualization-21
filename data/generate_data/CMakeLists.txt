cmake_minimum_required(VERSION 3.5)
project(visualization21)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)

# VTK
find_package(VTK COMPONENTS
  vtkCommonCore
  vtkCommonDataModel
  vtkIOLegacy
  vtkIOXML
  QUIET
)
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

# Eigen
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# OpenMP
option(USE_OpenMP                    "Use OpenMP"         ON)
if (USE_OpenMP)
    find_package(OpenMP)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif()
endif()

add_subdirectory(src)

add_executable(sphTest src/main_sph.cpp)
target_link_libraries(sphTest #
    PUBLIC vis21::utils #
    PUBLIC vis21::core #
    ${VTK_LIBRARIES} #
    Eigen3::Eigen
)

add_executable(cosmicwebTest src/main_cosmicweb.cpp)
target_link_libraries(cosmicwebTest #
    PUBLIC vis21::utils #
    PUBLIC vis21::core #
    ${VTK_LIBRARIES}
    Eigen3::Eigen
)

