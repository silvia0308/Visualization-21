#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

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
#include <stdio.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>

std::vector<int> vstar,vform,vwind,vAGN,vid;
std::string root_path;
using namespace std;

template <class TReader> vtkDataSet* ReadAnXMLFile(const char* fileName)
{
  vtkNew<TReader> reader;
  reader->SetFileName(fileName);
  reader->Update();
  reader->GetOutput()->Register(reader);
  return dynamic_cast<vtkDataSet*>(reader->GetOutput());
}

void run(int j){
    vtkSmartPointer<vtkDataSet> dataSet1;	
    std::string strid = std::to_string(j);
	//std::string name = "../../../data/Cosmology/Full.cosmo." + std::string(3-strid.length(),'0') + strid + ".vtp";
    std::string name = root_path + "/Full.cosmo." + std::string(3-strid.length(),'0') + strid + ".vtp";
    std::cout<<name<<std::endl;
    std::ofstream file;
    file.open("../type/"+ strid +".txt");

    

    dataSet1 = ReadAnXMLFile<vtkXMLPolyDataReader>(name.c_str());
    int numberOfPoints = dataSet1->GetNumberOfPoints();
    file<<numberOfPoints<<'\n';
    
    vtkPointData* pd1 = dataSet1->GetPointData();
    set<int> mask;
    int baryon = 0;
	int star = 0;
	int forming = 0;
	int wind = 0;
	int AGN = 0;
    vector<pair<int,int>> datatype;
	for (int i = 0; i < numberOfPoints; i++) {   
	  	int x = int( pd1->GetArray(10)->GetComponent(i, 0) ); 
        int id = int( pd1->GetArray(9)->GetComponent(i, 0) );     
        mask.insert(x);
        if( (x>>1)%2 == 1){
		  	baryon++;
		}
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
        //cout<< i << ' '<<id<<endl;
        datatype.push_back(make_pair(id,x));
    }
    sort(datatype.begin(),datatype.end());
    for( auto u : datatype ){
        int id = u.first;
        int x = u.second;
        file<<id<< ' '<<x<<'\n';
    }
    cout<<baryon<<' '<<star<<' '<<forming<<' '<<wind<<' '<<AGN<<endl;
    vstar.push_back(star);
    vform.push_back(forming);
    vwind.push_back(wind);
    vAGN.push_back(AGN);
    vid.push_back(j);

    for (auto it = mask.begin(); it !=mask.end(); ++it){
        cout <<*it<<' ';
    }cout<<endl;
        

    file.close();
}

int main(int argc, char** argv)
{
  root_path = argv[2];
  run( std::stoi(argv[1]) );
  /*std::ofstream file;
  file.open("../test.txt",std::ofstream::app);
  //file<<"Num-Star-FormingStar-WindStar-AGN\n";
  run( std::stoi(argv[1]) );
  file<<vid[0]<<' '<<vstar[0]<<' '<<vform[0]<<' '<<vwind[0]<<' '<<vAGN[0]<<'\n';
  file.close();*/

  return 0;
}
