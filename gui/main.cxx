#include <QApplication>
#include <QSurfaceFormat>

#include <QVTKOpenGLNativeWidget.h>

#include "MainWindow.h"

int main(int argc, char** argv)
{
  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);

  std::string root_path1, root_path2;
  std::cout << "Please input the path to cosmology folder: ";
  std::cin >> root_path1;
  std::cout << "Please input the path to txt result folder: ";
  std::cin >> root_path2;

  MainWindow mainWindow(root_path1,root_path2);
  mainWindow.show();

  return app.exec();
}
