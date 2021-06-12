#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointGaussianMapper.h>
#include <vtkNew.h>

#include "ParticleType.cxx"
#include "ParticleAttributes.cxx"
#include "CosWeb.cxx"

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);
  virtual ~MainWindow();
  void setRenderWindow(vtkSmartPointer<vtkRenderer> ren);
  void setFileId(int num);

private slots:
	void on_typeChecked();
	void on_timeChanged();

private:
  // Designer form
  Ui::MainWindow* ui;

  std::string previousState;
  int fileId;

  ParticleType* pt;
  ParticleAttributes* pa;
  CosWeb* cw;
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

};

#endif
