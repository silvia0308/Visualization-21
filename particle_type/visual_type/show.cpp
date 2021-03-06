#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPointGaussianMapper.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkExtractSelectedIds.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindowInteractor.h>

template <class TReader> vtkDataSet* ReadAnXMLFile(const char* fileName)
{
  vtkNew<TReader> reader;
  reader->SetFileName(fileName);
  reader->Update();
  reader->GetOutput()->Register(reader);
  return dynamic_cast<vtkDataSet*>(reader->GetOutput());
}

int main(int argc, char** argv)
{
	std::string root_path, time;
	std::cout << "Please input the path to cosmology folder: ";
	std::cin >> root_path;
	std::cout << "Please input the time step [0,312]: ";
	std::cin >> time;

	std::string strid = std::to_string(std::stoi(time)*2);
	//std::cout<<strid<<std::endl;
	std::string name = root_path+"/Full.cosmo." + std::string(3-strid.length(),'0') + strid + ".vtp";

	vtkSmartPointer<vtkDataSet> dataSet;	
	dataSet = ReadAnXMLFile<vtkXMLPolyDataReader>(name.c_str());
    int numberOfPoints = dataSet->GetNumberOfPoints();
    //std::cout <<  numberOfPoints << " points." << std::endl;

    vtkPointData* pd = dataSet->GetPointData();
    std::vector<int> star,form,wind,AGN,total,remain;
	std::vector<float> starType(numberOfPoints,0);
	int num = 0;
	int num2 = 0;
	for (int i = 0; i < numberOfPoints; i++) {   
		int x = int( pd->GetArray(10)->GetComponent(i, 0) ); 
		bool b = false;    
		if( (x>>5)%2 == 1){
			star.push_back(i);
			starType[num] = 1;
			b = true;
		}
		if( (x>>6)%2 == 1){
			wind.push_back(i);
			starType[num] = 3;
			b = true;
		}
		if( (x>>7)%2 == 1){
			form.push_back(i);
			starType[num] = 2;
			b = true;
		}
		if( (x>>8)%2 == 1){
			AGN.push_back(i);
			starType[num] = 4;
			b = true;
		}
		if(b){
			total.push_back(i);
			num++;
		}else{
			remain.push_back(i);
			num2++;
		}
    }
	vtkNew<vtkPointGaussianMapper> mapper;
	vtkNew<vtkPointGaussianMapper> mapper2;

  	vtkNew<vtkRenderer> renderer;
  	renderer->SetBackground(0.0, 0.0, 0.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  	renderWindow->SetSize(800, 800);
  	renderWindow->SetMultiSamples(0);
  	renderWindow->AddRenderer(renderer);

  	vtkNew<vtkActor> actor,actor2;
  	actor->SetMapper(mapper);
	actor2->SetMapper(mapper2);
	renderer->AddActor(actor2);
  	renderer->AddActor(actor);
	

  	vtkNew<vtkXMLPolyDataReader> reader;
  	reader->SetFileName(name.c_str());
  	reader->Update();


	vtkSmartPointer<vtkFloatArray> colors = vtkSmartPointer<vtkFloatArray>::New();
	colors->SetNumberOfValues(num);
	for(int i=0;i<num;i++){
		colors->SetValue(i,starType[i]);
	}

	vtkSmartPointer<vtkFloatArray> colors2 = vtkSmartPointer<vtkFloatArray>::New();
	colors2->SetNumberOfValues(num2);
	for(int i=0;i<num2;i++){
		colors2->SetValue(i,0);
	}

	vtkSmartPointer<vtkPolyData> data = reader->GetOutput();

	vtkNew<vtkIdTypeArray> ids,ids2;
  	ids->SetNumberOfComponents(1);
	ids2->SetNumberOfComponents(1);

  	for (int i: total){
    	ids->InsertNextValue(i);
  	}

	for (int i: remain){
    	ids2->InsertNextValue(i);
  	}

	// star data
  	vtkNew<vtkSelectionNode> selectionNode;
  	selectionNode->SetFieldType(vtkSelectionNode::POINT);
  	selectionNode->SetContentType(vtkSelectionNode::INDICES);
  	selectionNode->SetSelectionList(ids);

  	vtkNew<vtkSelection> selection;
  	selection->AddNode(selectionNode);

  	vtkNew<vtkExtractSelectedIds> extractSelectedIds;
  	extractSelectedIds->SetInputConnection(0, reader->GetOutputPort());
  	extractSelectedIds->SetInputData(1, selection);
  	extractSelectedIds->Update();

	vtkSmartPointer<vtkPolyData> stardata = vtkSmartPointer<vtkPolyData>::New();
    stardata->ShallowCopy(extractSelectedIds->GetOutput());
	stardata->GetPointData()->SetScalars(colors);

	// base data
	vtkNew<vtkSelectionNode> selectionNode2;
  	selectionNode2->SetFieldType(vtkSelectionNode::POINT);
  	selectionNode2->SetContentType(vtkSelectionNode::INDICES);
  	selectionNode2->SetSelectionList(ids2);

  	vtkNew<vtkSelection> selection2;
  	selection2->AddNode(selectionNode2);

  	vtkNew<vtkExtractSelectedIds> extractSelectedIds2;
  	extractSelectedIds2->SetInputConnection(0, reader->GetOutputPort());
  	extractSelectedIds2->SetInputData(1, selection2);
  	extractSelectedIds2->Update();

	vtkSmartPointer<vtkPolyData> basedata = vtkSmartPointer<vtkPolyData>::New();
    basedata->ShallowCopy(extractSelectedIds2->GetOutput());
	basedata->GetPointData()->SetScalars(colors2);

	vtkSmartPointer<vtkColorTransferFunction> colorTf = vtkSmartPointer<vtkColorTransferFunction>::New();
	colorTf -> AddRGBPoint(0,1,1,1);
	colorTf -> AddRGBPoint(1,1,0,0);
	colorTf -> AddRGBPoint(2,1,1,0);
	colorTf -> AddRGBPoint(3,0,1,0);
	colorTf -> AddRGBPoint(4,1,0,1);
	mapper->SetLookupTable(colorTf);
	mapper2->SetLookupTable(colorTf);
	mapper->SetInputData(stardata);
	mapper2->SetInputData(basedata);

	mapper->SetScaleFactor(0.2);
	mapper->EmissiveOff();
	mapper2->SetScaleFactor(0.01);
	mapper2->SetOpacityArray("RandomPointArray");
	mapper2->SetOpacityArrayComponent(0);
	mapper2->EmissiveOff();
	mapper2->SetTriangleScale(1.5);

	vtkNew<vtkPiecewiseFunction> otf;
	otf->AddPoint(-1.0, 0.01);
  	otf->AddPoint(0.0, 0.01);
  	otf->AddPoint(1.0, 0.01);
  	mapper2->SetScalarOpacityFunction(otf);

	// The render window interactor captures mouse events
	// and will perform appropriate camera or actor manipulation
	// depending on the nature of the events.
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// This starts the event loop and as a side effect causes an initial render.
	renderWindowInteractor->Start();

	return 0;
}
