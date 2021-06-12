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

#include <cstring>
#include "core/dataframe.h"

namespace vis21 {

template <class TReader> vtkDataSet* ReadAnXMLFile(const char* fileName)
{
  vtkNew<TReader> reader;
  reader->SetFileName(fileName);
  reader->Update();
  reader->GetOutput()->Register(reader);
  return dynamic_cast<vtkDataSet*>(reader->GetOutput());
}

// I modify FeatureVisual from JIAOYING to construct this function
void DataFrame::readFromFile(const std::string &filename) {
    vtkSmartPointer<vtkDataSet> dataset;
    dataset = ReadAnXMLFile<vtkXMLPolyDataReader>(filename.c_str());

    int n_points = dataset->GetNumberOfPoints();
    vtkPointData* pd = dataset->GetPointData();
    for (int i = 0; i < n_points; ++i) {
        double p[3]; dataset->GetPoint(i, p);
        Particle ptc(Eigen::Vector3d(p[0], p[1], p[2]));
        ptc.v = Eigen::Vector3d(pd->GetArray(0)->GetComponent(i, 0),
                                pd->GetArray(1)->GetComponent(i, 0),
                                pd->GetArray(2)->GetComponent(i, 0)
                                );
        ptc.set_attr(ParticleType::MASS, pd->GetArray(3)->GetComponent(i, 0));
        ptc.set_attr(ParticleType::UU, pd->GetArray(4)->GetComponent(i, 0));
        ptc.set_attr(ParticleType::HH, pd->GetArray(5)->GetComponent(i, 0));
        ptc.set_attr(ParticleType::MU, pd->GetArray(6)->GetComponent(i, 0));
        ptc.set_attr(ParticleType::RHO, pd->GetArray(7)->GetComponent(i, 0));
        ptc.set_attr(ParticleType::PHI, pd->GetArray(8)->GetComponent(i, 0));
        ptc.id = pd->GetArray(9)->GetComponent(i, 0);
        ptc.mask = pd->GetArray(10)->GetComponent(i, 0);

        // advanced attributes
        double a = 1.0 / 201.0 + (1.0 - 1.0 / 201.0) * frame_id / 625.0;
        double z = (1 / a - 1);
        double T = 4.8 * 1e5 * ptc.get_attr(ParticleType::UU) / ((1 + z) * (1 + z) * (1 + z));
        ptc.set_attr(ParticleType::TEMPERATURE, T);
        double S = log(T) - 2.0 / 3.0 * log(ptc.get_attr(ParticleType::RHO));
        ptc.set_attr(ParticleType::ENTROPY, S);
        particles.push_back(ptc);
    }
}

}

