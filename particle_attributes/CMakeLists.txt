
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(SimpleRayCast)

find_package(VTK COMPONENTS 
  vtkCommonColor
  vtkCommonCore
  vtkCommonDataModel
  vtkIOLegacy
  vtkInteractionStyle
  vtkRenderingContextOpenGL2
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2
  vtkRenderingVolume
  vtkRenderingVolumeOpenGL2
  QUIET
)

find_package(VTK REQUIRED)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
	${PROJECT_NAME}.cxx )

target_link_libraries(${PROJECT_NAME} 
	PRIVATE
		${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES})
