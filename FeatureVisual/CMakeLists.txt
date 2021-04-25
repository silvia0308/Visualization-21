
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(FeatureVisual)

find_package(VTK COMPONENTS 
  vtkCommonCore
  vtkCommonDataModel
  vtkIOLegacy
  vtkIOXML
  QUIET
)

if (NOT VTK_FOUND)
  message("Skipping FeatureVisual: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(FeatureVisual MACOSX_BUNDLE FeatureVisual.cxx )
  target_link_libraries(FeatureVisual PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(FeatureVisual MACOSX_BUNDLE FeatureVisual.cxx )
  target_link_libraries(FeatureVisual PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS FeatureVisual
    MODULES ${VTK_LIBRARIES}
    )
endif ()
