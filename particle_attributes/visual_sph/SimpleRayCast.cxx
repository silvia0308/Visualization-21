#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkMetaImageReader.h>
#include <vtkImageData.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h> 
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>


bool boundPoint(int gridsize, int x, int y, int z) {
    if (y == 0 && z == 0 || x == 0 && z == 0 || x == 0 && y==0 || x == gridsize - 1 && z == 0 || y == gridsize - 1 && z == 0 || x == gridsize - 1 && y == 0 || z == gridsize - 1 && y == 0 || z == gridsize - 1 && x == 0 || y == gridsize - 1 && x == 0 || z == gridsize - 1 && x == gridsize - 1 || z == gridsize - 1 && y == gridsize - 1 || x == gridsize - 1 && y == gridsize - 1) {
        return true;
    }
    else {
        return false;
    }
}

int main(int argc, char* argv[])
{
  std::string root_path, feature, time;
  std::cout << "Please input the path to the sph txt result folder: ";
  std::cin >> root_path;
  std::cout << "Please input the attribute name (density/interEn/gravPo/curlnorm/temperature/entropy/divergence): ";
  std::cin >> feature;
  std::cout << "Please input the time step [0,312]: ";
  std::cin >> time;
  //std::string strid = std::to_string(312);

  //std::string filepath = "C:/Users/jiaoy/Desktop/cosmo_200_2_wc4/sph_200_hh_624_" + feature + "_WC4.txt";
  std::string filepath = root_path + "/sph_result_" + time + "_" + feature + ".txt";
  std::ifstream input(filepath);
  int gridsize;
  input>>gridsize;
  double corner1x,corner1y,corner1z;
  input>>corner1x>>corner1y>>corner1z;
  double corner2x,corner2y,corner2z;
  input>>corner2x>>corner2y>>corner2z;
  std::vector<double> value;
  double t;
  double maxt = 0;
  for(int i=0;i<gridsize*gridsize*gridsize;i++){
    input>>t;
    value.push_back(t);
    maxt = std::max(maxt,t);
  }
  
  //std::cout<<"Grid Size: "<<gridsize<<std::endl;
  //std::cout<<maxt<<std::endl;


  vtkNew<vtkImageData> imageData;
  imageData->SetDimensions(gridsize, gridsize, gridsize);
  imageData->AllocateScalars(VTK_FLOAT, 1);

  // Fill every entry of the image data with a color
  int* dims = imageData->GetDimensions();

  float* ptr =
      static_cast<float*>(imageData->GetScalarPointer(0, 0, 0));
  for (int z = 0; z < dims[2]; z++)
  {
      for (int y = 0; y < dims[1]; y++)
      {
          for (int x = 0; x < dims[0]; x++)
          {
              int idx = x * gridsize * gridsize + y * gridsize + z;
			  			  
			  //set value for the boundary box 
			  if (boundPoint(gridsize, x, y, z) && feature == "divergence") {
                *ptr++ = 1000000;
              }
			  else if(boundPoint(gridsize, x, y, z) && feature == "gravPo"){
                *ptr++ = 10000;
              }
              else if (boundPoint(gridsize, x, y, z)) {
                *ptr++ = -100;
              }
              else {
                  if (feature == "density") {
                      *ptr++ = (int)sqrt(value[idx]) / 100000;
                  }
                  if (feature == "interEn") {
                      *ptr++ = (int)sqrt(10*value[idx]) / 10;
                  }
                  if (feature == "curlnorm") {
                      *ptr++ = (int)value[idx] / 10;
                  }
                  if (feature == "temperature") {
                      *ptr++ = (int)sqrt(value[idx]) / 1000;
                  }
                  if (feature == "entropy") {
                      *ptr++ = (int)value[idx];
                  }
				  if (feature == "gravPo" || feature == "divergence") {
                      *ptr++ = value[idx] ;
                  }
              }
          }
      }
  }


  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);


  // Create transfer mapping scalar value to opacity
  vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
  if (feature == "density") {
      opacityTransferFunction->AddPoint(-100, 1);
      opacityTransferFunction->AddPoint(0, 0.0);
      opacityTransferFunction->AddPoint(0.5, 0.02);
      opacityTransferFunction->AddPoint(1, 0.03);
      opacityTransferFunction->AddPoint(2, 0.05);
      opacityTransferFunction->AddPoint(10, 0.8);
      opacityTransferFunction->AddPoint(4000, 1);
  }
  if (feature == "interEn") {
      opacityTransferFunction->AddPoint(-100, 1);
      opacityTransferFunction->AddPoint(0, 0.0);
      opacityTransferFunction->AddPoint(10, 0.02);
      opacityTransferFunction->AddPoint(30, 0.05);
      opacityTransferFunction->AddPoint(50, 0.2);
      opacityTransferFunction->AddPoint(100, 0.8);
      opacityTransferFunction->AddPoint(900, 1);
  }
  if (feature == "gravPo") {
      opacityTransferFunction->AddPoint(-50, 0.1);
      opacityTransferFunction->AddPoint(0, 0);
      opacityTransferFunction->AddPoint(50, 0.1);
      opacityTransferFunction->AddPoint(2000, 0.1);
      opacityTransferFunction->AddPoint(3000, 1);
  }
  if (feature == "curlnorm") {
      opacityTransferFunction->AddPoint(-100, 1);
      opacityTransferFunction->AddPoint(0, 0.0);
      opacityTransferFunction->AddPoint(50, 0.02);
      opacityTransferFunction->AddPoint(100, 0.2);
      opacityTransferFunction->AddPoint(200, 0.7);
      opacityTransferFunction->AddPoint(300, 0.8);
      opacityTransferFunction->AddPoint(600, 1);
      opacityTransferFunction->AddPoint(3900, 1);
  }
  if (feature == "temperature") {
      opacityTransferFunction->AddPoint(-100, 1);
      opacityTransferFunction->AddPoint(0, 0.0);
      opacityTransferFunction->AddPoint(2, 0.0);
      opacityTransferFunction->AddPoint(10, 0.02);
      opacityTransferFunction->AddPoint(100, 0.1);
      opacityTransferFunction->AddPoint(200, 0.7);
      opacityTransferFunction->AddPoint(300, 0.8);
      opacityTransferFunction->AddPoint(600, 1);
  }
  if (feature == "entropy") {
      opacityTransferFunction->AddPoint(-100, 1);
      opacityTransferFunction->AddPoint(0, 0.0);
      opacityTransferFunction->AddPoint(2, 0.03);
      opacityTransferFunction->AddPoint(5, 0.3);
      opacityTransferFunction->AddPoint(7, 0.8);
      opacityTransferFunction->AddPoint(10, 1);
  }
  if (feature == "divergence") {
      opacityTransferFunction->AddPoint(-2000, 1);
      opacityTransferFunction->AddPoint(-500, 0.5);
      opacityTransferFunction->AddPoint(-300, 0.05);
      opacityTransferFunction->AddPoint(-50, 0);
      opacityTransferFunction->AddPoint(0, 0);
      opacityTransferFunction->AddPoint(50, 0);
      opacityTransferFunction->AddPoint(300, 0.05);
      opacityTransferFunction->AddPoint(500, 0.2);
      opacityTransferFunction->AddPoint(2000, 0.8);
      opacityTransferFunction->AddPoint(8000, 0);
      opacityTransferFunction->AddPoint(10000, 1);
  }



  // Create transfer mapping scalar value to color
  vtkNew<vtkColorTransferFunction> colorTransferFunction;
  if (feature == "density") {
      colorTransferFunction->AddRGBPoint(-100.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(2, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(10, 1, 1, 0);
      colorTransferFunction->AddRGBPoint(50, 0, 1, 0);
      colorTransferFunction->AddRGBPoint(4000, 0, 1, 0);
  }
  if (feature == "interEn") {
      colorTransferFunction->AddRGBPoint(-100.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(2, 1, 1, 1);     
      colorTransferFunction->AddRGBPoint(50, 0.929, 0.4178, 0.02);
      colorTransferFunction->AddRGBPoint(100, 0.6, 0.2705, 0.02);
      colorTransferFunction->AddRGBPoint(900, 0.6, 0.2705, 0.02);
  }
  if (feature == "gravPo") {
      colorTransferFunction->AddRGBPoint(-100.0, 0, 1, 0);
      colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(30, 1, 0, 0);
      colorTransferFunction->AddRGBPoint(80, 1, 0, 0);
      colorTransferFunction->AddRGBPoint(2000, 1, 0, 0);
      colorTransferFunction->AddRGBPoint(3000, 1, 1, 1);
  }
  if (feature == "curlnorm") {
      colorTransferFunction->AddRGBPoint(-100.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(50, 0.7686, 0.4118, 0.8588);
      colorTransferFunction->AddRGBPoint(200, 0.7686, 0.4118, 0.8588);
      colorTransferFunction->AddRGBPoint(300, 0.5529, 0.0275, 0.6902);
      colorTransferFunction->AddRGBPoint(3900, 0.5529, 0.0275, 0.6902);
  }
  if (feature == "temperature") {
      colorTransferFunction->AddRGBPoint(-100.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(10, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(30, 1, 1, 0.6);
      colorTransferFunction->AddRGBPoint(100, 1, 1, 0.2);
      colorTransferFunction->AddRGBPoint(300, 1, 1, 0);
      colorTransferFunction->AddRGBPoint(600, 1, 1, 0);
  }
  if (feature == "entropy") {
      colorTransferFunction->AddRGBPoint(-100.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(2, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(3, 0.5294, 1, 0.6353);
      colorTransferFunction->AddRGBPoint(5, 0, 1, 0.2235);
      colorTransferFunction->AddRGBPoint(7, 0.102, 0.5412, 0.2039);
      colorTransferFunction->AddRGBPoint(10, 0.102, 0.5412, 0.2039);
  }
  if (feature == "divergence") {
	  colorTransferFunction->AddRGBPoint(-2000.0, 0, 0, 1);
      colorTransferFunction->AddRGBPoint(-500.0, 0.1, 0.3, 1);
      colorTransferFunction->AddRGBPoint(-300.0, 0.2, 0.5, 0.8);
      colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
      colorTransferFunction->AddRGBPoint(500, 0.8, 0.3, 0);
      colorTransferFunction->AddRGBPoint(2000, 1, 0, 0);
      colorTransferFunction->AddRGBPoint(8000, 1, 0, 0);
      colorTransferFunction->AddRGBPoint(10000, 1, 1, 1);
  }
 

  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTransferFunction);
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();



  vtkNew<vtkFixedPointVolumeRayCastMapper> volumeMapper;
  volumeMapper->SetInputData(imageData);


  vtkNew<vtkVolume> volume;
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  ren1->AddVolume(volume);
  ren1->SetBackground(0.0,0.0,0.0);
  ren1->GetActiveCamera()->Azimuth(25);
  //ren1->GetActiveCamera()->Elevation(30);
  ren1->ResetCameraClippingRange();
  ren1->ResetCamera();

  renWin->SetSize(600, 600);
  renWin->SetWindowName("SimpleRayCast");
  renWin->Render();

/*
  //Write to image
  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renWin);
  windowToImageFilter->Update();
  vtkNew<vtkPNGWriter> writer; 
  writer->SetInputData(windowToImageFilter->GetOutput()); 
  std::string name = "out_200_hh_WC4.png";
  writer->SetFileName(name.c_str());
  writer->Update();
  writer->Write();
*/
 
 

  iren->Start();

  return EXIT_SUCCESS;
}
