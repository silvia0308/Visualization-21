
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
  cmake_policy(SET CMP0071 NEW)
endif()

PROJECT(Visualization-21)

find_package(VTK REQUIRED)

if(NOT VTK_FOUND)
  message(FATAL_ERROR "Visualization-21: ${VTK_NOT_FOUND_MESSAGE}")
endif()

if(NOT(vtkGUISupportQt_LOADED OR TARGET VTK::GUISupportQt))
  message(FATAL_ERROR "Visualization-21: VTK not built with Qt support.")
endif()

if(NOT DEFINED VTK_QT_VERSION)
  set(VTK_QT_VERSION 5)
endif()

set(qt_components Core Gui Widgets)
if(${VTK_QT_VERSION} VERSION_GREATER_EQUAL 6)
  list(APPEND qt_components OpenGLWidgets)
endif()
list(SORT qt_components)
# We have ui files, so this will also bring in the macro:
#   qt5_wrap_ui or qt_wrap_ui from Widgets.
find_package(Qt${VTK_QT_VERSION} QUIET
  REQUIRED COMPONENTS ${qt_components}
)

foreach(_qt_comp IN LISTS qt_components)
  list(APPEND qt_modules "Qt${VTK_QT_VERSION}::${_qt_comp}")
endforeach()

message (STATUS "VTK_VERSION: ${VTK_VERSION}, Qt Version: ${Qt${VTK_QT_VERSION}Widgets_VERSION}")

if (VTK_VERSION VERSION_LESS "8.90.0")
# Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
else()
# Instruct CMake to run moc and uic automatically when needed.
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.cxx)

# For VTK versions greater than or equal to 8.90.0:
#  CMAKE_AUTOUIC is ON so we handle uic automatically for Qt targets.
#  CMAKE_AUTOMOC is ON so we handle moc automatically for Qt targets.
# However we have to do the following for VTK versions less than 8.90.0,
#  and we also assume, in this case, that Qt5 is being used.
if (VTK_VERSION VERSION_LESS "8.90.0")
  qt5_wrap_ui(UISrcs ${EXAMPLE}.ui)
endif()

if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  # CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
  add_executable(MainWindow MACOSX_BUNDLE
    ${CXX_FILES} ${UISrcs} ${QT_WRAP})
  if (Qt${VTK_QT_VERSION}Widgets_VERSION VERSION_LESS "5.11.0")
    qt5_use_modules(MainWindow ${qt_components})
  else()
    target_link_libraries(MainWindow ${qt_modules})
  endif()
  target_link_libraries(MainWindow ${VTK_LIBRARIES})
else()
  # CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
  add_executable(MainWindow MACOSX_BUNDLE
    ${CXX_FILES} ${UISrcs} ${QT_WRAP})
  if (Qt${VTK_QT_VERSION}Widgets_VERSION VERSION_LESS "5.11.0")
    qt5_use_modules(MainWindow ${qt_components})
  else()
    target_link_libraries(MainWindow ${qt_modules})
  endif()
  target_link_libraries(MainWindow ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS MainWindow
    MODULES ${VTK_LIBRARIES}
  )
endif()
