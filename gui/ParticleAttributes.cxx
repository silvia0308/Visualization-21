#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkMetaImageReader.h>
#include <vtkImageData.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h> 

class ParticleAttributes {
private:
    vtkSmartPointer<vtkRenderer> ren1;
    vtkSmartPointer<vtkImageData> imageData;
    vtkNew<vtkVolume> volume;
    vtkNew<vtkFixedPointVolumeRayCastMapper> volumeMapper;
    vtkNew<vtkVolumeProperty> volumeProperty;
    vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
    vtkNew<vtkColorTransferFunction>colorTransferFunction;

    std::string feature="density";
    int fileId=0;

public:
    bool boundPoint(int gridsize, int x, int y, int z) {
        if (y == 0 && z == 0 || x == 0 && z == 0 || x == 0 && y == 0 || x == gridsize - 1 && z == 0 || y == gridsize - 1 && z == 0 || x == gridsize - 1 && y == 0 || z == gridsize - 1 && y == 0 || z == gridsize - 1 && x == 0 || y == gridsize - 1 && x == 0 || z == gridsize - 1 && x == gridsize - 1 || z == gridsize - 1 && y == gridsize - 1 || x == gridsize - 1 && y == gridsize - 1) {
            return true;
        }
        else {
            return false;
        }
    };

    void initialize() {
        // The property describes how the data will look
        volumeProperty->SetColor(colorTransferFunction);
        volumeProperty->SetScalarOpacity(opacityTransferFunction);
        volumeProperty->ShadeOn();
        volumeProperty->SetInterpolationTypeToLinear();

        // The mapper / ray cast function know how to render the data
        volumeMapper->SetInputData(imageData);

        // The volume holds the mapper and the property and
        // can be used to position/orient the volume
        //volume= vtkSmartPointer<vtkVolume>::New();
        volume->SetMapper(volumeMapper);
        volume->SetProperty(volumeProperty);

        ren1 = vtkSmartPointer<vtkRenderer>::New();
        ren1->AddVolume(volume);
        ren1->SetBackground(0.0, 0.0, 0.0);
        ren1->GetActiveCamera()->Azimuth(25);
        ren1->ResetCameraClippingRange();
        ren1->ResetCamera();
    };

    vtkSmartPointer<vtkRenderer> getRenderer() {
        return ren1;
    };

    void setImageData() {
        std::string strid = std::to_string(fileId);
        std::string name = "C:/Visualization-21/data/sph_result_" + strid + "_" + feature + ".txt";
        std::ifstream input(name);
        int gridsize;
        input >> gridsize;
        double corner1x, corner1y, corner1z;
        input >> corner1x >> corner1y >> corner1z;
        double corner2x, corner2y, corner2z;
        input >> corner2x >> corner2y >> corner2z;
        std::vector<double> value;
        std::vector<float> scaled;
        double t;
        double maxt = 0;
        for (int i = 0; i < gridsize * gridsize * gridsize; i++) {
            input >> t;
            int z = i % gridsize;
            int y = (i / gridsize) % gridsize;
            int x = (i / (gridsize * gridsize)) % gridsize;
            value.push_back(t);
            maxt = std::max(maxt, t);
        }
        std::cout << "Grid Size: " << gridsize << std::endl;
        std::cout << maxt << std::endl;
        // This is a simple volume rendering example that
        // uses a vtkFixedPointVolumeRayCastMapper
        imageData = vtkSmartPointer<vtkImageData>::New();
        //vtkNew<vtkImageData> imageData;
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
                    if (boundPoint(gridsize, x, y, z)) {
                        *ptr++ = -100;
                    }
                    else {
                        if (feature == "density") {
                            *ptr++ = (int)sqrt(value[idx]) / 100000;
                        }
                        if (feature == "interEn") {
                            *ptr++ = (int)sqrt(10 * value[idx]) / 10;
                        }
                        if (feature == "gravPo") {
                            *ptr++ = value[idx] + 674.561;
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
                        if (feature == "divergence") {
                            *ptr++ = (int)value[idx] + 35200;
                        }
                    }
                }
            }
        }
    };

    void setVolume() {
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
        /*
          if (feature == "gravPo") {
              opacityTransferFunction->AddPoint(-100, 1);
              opacityTransferFunction->AddPoint(0, 0.2);
              //opacityTransferFunction->AddPoint(100, 1);
              //opacityTransferFunction->AddPoint(200, 0.05);
              opacityTransferFunction->AddPoint(500, 0.8);
              //opacityTransferFunction->AddPoint(600, 1);
              //opacityTransferFunction->AddPoint(600, 1);
              opacityTransferFunction->AddPoint(770, 1);
          }
          */
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
            //opacityTransferFunction->AddPoint(300, 0.8);
            //opacityTransferFunction->AddPoint(600, 1);
        }

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
        /*
          if (feature == "gravPo") {
              colorTransferFunction->AddRGBPoint(-100.0, 1, 1, 1);
              colorTransferFunction->AddRGBPoint(0.0, 0, 1, 0);
              colorTransferFunction->AddRGBPoint(770, 1, 1, 1);
              //colorTransferFunction->AddRGBPoint(100, 0, 1, 0);
              //colorTransferFunction->AddRGBPoint(200, 1, 1, 1);
              //colorTransferFunction->AddRGBPoint(500, 1, 1, 1);
              //colorTransferFunction->AddRGBPoint(600, 0.6392, 0.0588, 0.3882);
              //colorTransferFunction->AddRGBPoint(770, 0.6392, 0.0588, 0.3882);
          }
        */
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
            //colorTransferFunction->AddRGBPoint(600, 1, 1, 0);
        }

        volume->Update();
    };

    void removePoints() {
        opacityTransferFunction->RemoveAllPoints();
        colorTransferFunction->RemoveAllPoints();
    };

    void setAll(int fileId, std::string feature) {
        this->fileId = fileId;
        this->feature = feature;
        std::string strid = std::to_string(fileId);
        setImageData();
        volumeMapper->SetInputData(imageData);
        removePoints();
        setVolume();
    };

    void changeFile(int fileId) {
        this->fileId = fileId;
        setImageData();
        volumeMapper->SetInputData(imageData);
    };
};
