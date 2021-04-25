#include <vtkCellData.h>
#include <vtkCellTypes.h>
#include <vtkDataSet.h>
#include <vtkDataSetReader.h>
#include <vtkFieldData.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRectilinearGrid.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLCompositeDataReader.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtksys/SystemTools.hxx>

#include <string.h>
#include <vector>
#include <map>

namespace {

template <class TReader> vtkDataSet* ReadAnXMLFile(const char* fileName)
{
  vtkNew<TReader> reader;
  reader->SetFileName(fileName);
  reader->Update();
  reader->GetOutput()->Register(reader);
  return dynamic_cast<vtkDataSet*>(reader->GetOutput());
}

} // namespace

int main()
{  
  // Process each file on the command line
  std::string root_path = "C:\\Users\\jiaoy\\Desktop\\ETH-IN\\Visualization\\project\\Cosmology\\";
  int total_file = 313;
  int f = 0;
  
  std::vector<std::vector<float>> internalEnergy;
  std::vector<std::vector<float>> density;
  std::vector<std::vector<float>> graviPotential;
  
  while (f < total_file)
  {
    int padding = 3 - std::to_string(f * 2).length();
    std::string file_name;
    if (padding == 2) {
        file_name = "00" + std::to_string(f * 2);
    }
    else if (padding == 1) {
        file_name = "0" + std::to_string(f * 2);
    }
    else {
        file_name = std::to_string(f * 2);
    }
    std::string file_path = root_path + "Full.cosmo." + file_name + ".vtp"; 
    vtkSmartPointer<vtkDataSet> dataSet;	
	dataSet = ReadAnXMLFile<vtkXMLPolyDataReader>(file_path.c_str());

    int numberOfPoints = dataSet->GetNumberOfPoints();

    // Generate a report
    std::cout << "------------------------" << std::endl;
    std::cout << file_path << std::endl
              << " contains a " << std::endl
              << dataSet->GetClassName() << " that has " << numberOfPoints
              << " points." << std::endl;
			  

    // Now check for point data
    vtkPointData* pd = dataSet->GetPointData();
    if (pd)
    {
      std::cout << " contains point data with " << pd->GetNumberOfArrays()
                << " arrays." << std::endl;
					  
	  
      for (int i = 0; i < pd->GetNumberOfArrays(); i++)
      {
        std::cout << "\tArray " << i << " is named "
                  << (pd->GetArrayName(i) ? pd->GetArrayName(i) : "NULL")
                  << " has " << pd->GetArray(i)->GetNumberOfTuples()
                  << " tuples"
                  << " with " << pd->GetArray(i)->GetNumberOfComponents()
                  << " components"
                  << " of type " << pd->GetArray(i)->GetClassName()
                  << std::endl;
      }
	  
      std::vector<float> interEn;
      std::vector<float> dens;
      std::vector<float> gravPo;
      for (int i = 0; i < numberOfPoints; i++) {          
          interEn.push_back(pd->GetArray(4)->GetComponent(i, 0));
          dens.push_back(pd->GetArray(7)->GetComponent(i, 0));
          gravPo.push_back(pd->GetArray(8)->GetComponent(i, 0));
      }
	  internalEnergy.push_back(interEn);
	  density.push_back(dens);
	  graviPotential.push_back(gravPo);
    }
   
    f++;
  }
  return EXIT_SUCCESS;
}
