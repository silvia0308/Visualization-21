#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPointGaussianMapper.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkGenericOpenGLRenderWindow.h>

class CosWeb {
private:
    std::string root_path;
    vtkSmartPointer<vtkPolyData> data;
    vtkNew<vtkPointGaussianMapper> mapper;
    vtkNew<vtkRenderer> renderer;

public:
    CosWeb(std::string txt_results) {
        root_path = txt_results;
    };

    template <class TReader> vtkDataSet* ReadAnXMLFile(const char* fileName)
    {
        vtkNew<TReader> reader;
        reader->SetFileName(fileName);
        reader->Update();
        reader->GetOutput()->Register(reader);
        return dynamic_cast<vtkDataSet*>(reader->GetOutput());
    }

    void getData(int fileId) {
        std::string strid = std::to_string(fileId);
        std::string filename = root_path+"/cosmicweb_result_" + strid + ".txt";
        std::ifstream input(filename);

        vtkNew<vtkPoints> points;
        int num;
        input >> num;
        double x, y, z;
        for (int i = 0; i < num; i++) {
            input >> x >> y >> z;
            double sc = 1;
            points->InsertNextPoint(sc * x, sc * y, sc * z);
        }
        data= vtkSmartPointer<vtkPolyData>::New();
        data->SetPoints(points);
    };

    void initialize() {
        getData(0);
        mapper->SetInputData(data);
        mapper->SetScaleFactor(0.02);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        renderer->SetBackground(0.0, 0.0, 0.0);
        renderer->AddActor(actor);
    };

    vtkSmartPointer<vtkRenderer> getRenderer() {
        return renderer;
    };

    void setMapper() {
        mapper->SetInputData(data);
    };
};
