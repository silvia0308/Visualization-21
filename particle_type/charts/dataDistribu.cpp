#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkCellData.h>
#include <vtkCellTypes.h>
#include <vtkDataSet.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>

std::vector<int> vstar,vform,vwind,vAGN,vid;
std::string root_path;

template <class TReader> vtkDataSet* ReadAnXMLFile(const char* fileName)
{
  vtkNew<TReader> reader;
  reader->SetFileName(fileName);
  reader->Update();
  reader->GetOutput()->Register(reader);
  return dynamic_cast<vtkDataSet*>(reader->GetOutput());
}

void run(int j){
	std::string strid = std::to_string(j);
	std::string name = root_path + "/Full.cosmo." + std::string(3 - strid.length(), '0') + strid + ".vtp";
	std::cout << name << std::endl;
	vtkSmartPointer<vtkDataSet> dataSet;
    dataSet = ReadAnXMLFile<vtkXMLPolyDataReader>(name.c_str());
    int numberOfPoints = dataSet->GetNumberOfPoints();

    vtkPointData* pd = dataSet->GetPointData();
	int star = 0;
	int forming = 0;
	int wind = 0;
	int AGN = 0;
	for (int i = 0; i < numberOfPoints; i++) {   
	int x = int( pd->GetArray(10)->GetComponent(i, 0) );     
    //mask.push_back(x);
		if( (x>>5)%2 == 1){
		star++;
		}
		if( (x>>6)%2 == 1){
		wind++;
		}
		if( (x>>7)%2 == 1){
		forming++;
		}
		if( (x>>8)%2 == 1){
		AGN++;
		}
		
    }
    vstar.push_back(star);
    vform.push_back(forming);
    vwind.push_back(wind);
    vAGN.push_back(AGN);
    vid.push_back(j);
}

int main(int argc, char** argv)
{
	std::cout << "Please input the path to cosmology folder: ";
	std::cin >> root_path;
  
  std::ofstream file;
  file.open("../distribute.txt",std::ofstream::app);
  file<<"Num-Star-FormingStar-WindStar-AGN\n";
  for(int j=614;j<=624;j+=2){
    run(j);
  }
  for(int i=0;i<vstar.size();i++){
    file<<vid[i]<<' '<<vstar[i]<<' '<<vform[i]<<' '<<vwind[i]<<' '<<vAGN[i]<<'\n';
  }
  file.close();

  return 0;
}
