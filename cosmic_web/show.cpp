#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPointGaussianMapper.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

template <class TReader> vtkDataSet* ReadAnXMLFile(const char* fileName)
{
  vtkNew<TReader> reader;
  reader->SetFileName(fileName);
  reader->Update();
  reader->GetOutput()->Register(reader);
  return dynamic_cast<vtkDataSet*>(reader->GetOutput());
}

int main(int argc, char* argv[])
{
	std::string root_path, time;
	std::cout << "Please input the path to the cosmic web txt result folder: ";
	std::cin >> root_path;
	std::cout << "Please input the time step [0,312]: ";
	std::cin >> time;

	std::string filename = root_path+"/cosmicweb_result_"+time+".txt";
	std::ifstream input(filename);

	vtkNew<vtkPoints> points;
	int num;
  	input>>num;
	double x,y,z;
	for (int i = 0; i < num; i++) {
		input >> x >> y >> z;
		double sc = 1;
		points->InsertNextPoint(sc * x, sc * y, sc * z);
	}
	
	vtkNew<vtkPolyData> data;
  	data->SetPoints(points);

	vtkNew<vtkPointGaussianMapper> mapper;

  	vtkNew<vtkRenderer> renderer;
  	renderer->SetBackground(0.0, 0.0, 0.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  	renderWindow->SetSize(800,800);
  	renderWindow->SetMultiSamples(0);
  	renderWindow->AddRenderer(renderer);

  	vtkNew<vtkActor> actor;
  	actor->SetMapper(mapper);
  	renderer->AddActor(actor);

	mapper->SetInputData(data);
	//radius
  	mapper->SetScaleFactor(0.02);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Start();

	return 0;
}