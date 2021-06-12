#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vtkGenericOpenGLRenderWindow.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

// Constructor
MainWindow::MainWindow(std::string root_path1, std::string root_path2, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), pt(new ParticleType(root_path1)),pa(new ParticleAttributes(root_path2)),cw(new CosWeb(root_path2))
{
    this->ui->setupUi(this);

    this->setWindowState(Qt::WindowMaximized);

    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->checkBox_2, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->radioButton, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->radioButton_2, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->radioButton_3, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->radioButton_4, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->radioButton_5, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->radioButton_6, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->radioButton_7, SIGNAL(clicked(bool)), this, SLOT(on_typeChecked()));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(on_timeChanged()));

    vtkObject::GlobalWarningDisplayOff();

    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderWindow->SetSize(300, 300);
    renderWindow->SetMultiSamples(0);

    pt->initialize();
    pa->setAll(0, "interEn");
    pa->initialize();
    cw->initialize();
};

MainWindow::~MainWindow()
{
    delete this->ui;
};

void MainWindow::on_typeChecked() {
    //std::cout << ui->horizontalSlider->value() << std::endl;
    //Cosmic Web
    if (previousState != ui->checkBox->text().toStdString() && ui->checkBox->isChecked()) {
        //std::cout << ui->checkBox->text().toStdString() << std::endl;
        previousState = ui->checkBox->text().toStdString();
        cw->getData(ui->horizontalSlider->value());
        cw->setMapper();
        setRenderWindow(cw->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Particle Type
    if (previousState != ui->checkBox_2->text().toStdString() && ui->checkBox_2->isChecked()) {
        //std::cout << ui->checkBox_2->text().toStdString() << std::endl;
        previousState = ui->checkBox_2->text().toStdString();
        setFileId(ui->horizontalSlider->value()*2);
        pt->getStarData(fileId);
        pt->setMappers();
        setRenderWindow(pt->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Internal Energy
    if (previousState != ui->radioButton->text().toStdString() && ui->radioButton->isChecked()) {
        //std::cout << ui->radioButton->text().toStdString() << std::endl;
        previousState = ui->radioButton->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "interEn");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Density
    if (previousState != ui->radioButton_2->text().toStdString() && ui->radioButton_2->isChecked()) {
        //std::cout << ui->radioButton_2->text().toStdString() << std::endl;
        previousState = ui->radioButton_2->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "density");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Temperature
    if (previousState != ui->radioButton_4->text().toStdString() && ui->radioButton_4->isChecked()) {
        //std::cout << ui->radioButton_4->text().toStdString() << std::endl;
        previousState = ui->radioButton_4->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "temperature");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Entropy
    if (previousState != ui->radioButton_5->text().toStdString() && ui->radioButton_5->isChecked()) {
        //std::cout << ui->radioButton_5->text().toStdString() << std::endl;
        previousState = ui->radioButton_5->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "entropy");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Curl Magnitude
    if (previousState != ui->radioButton_7->text().toStdString() && ui->radioButton_7->isChecked()) {
        //std::cout << ui->radioButton_7->text().toStdString() << std::endl;
        previousState = ui->radioButton_7->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "curlnorm");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Divergence
    if (previousState != ui->radioButton_6->text().toStdString() && ui->radioButton_6->isChecked()) {
        //std::cout << ui->radioButton_6->text().toStdString() << std::endl;
        previousState = ui->radioButton_6->text().toStdString();
        visualize("divergence",getData(ui->horizontalSlider->value(),"divergence"));
    }
    //Gravitational Potential
    if (previousState != ui->radioButton_3->text().toStdString() && ui->radioButton_3->isChecked()) {
        //std::cout << ui->radioButton_3->text().toStdString() << std::endl;
        previousState = ui->radioButton_3->text().toStdString();
        visualize("gravPo",getData(ui->horizontalSlider->value(),"gravPo"));
    }
};

void MainWindow::on_timeChanged() {
    //std::cout<<ui->horizontalSlider->value()<<std::endl;
    if (previousState == "Cosmic Web") {
        cw->getData(ui->horizontalSlider->value());
        cw->setMapper();
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    if (previousState== "Particle Type") {
        setFileId(ui->horizontalSlider->value() * 2);
        pt->getStarData(fileId);
        pt->setMappers();
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    if (previousState == "Internal Energy"|| previousState == "Density"|| previousState == "Temperature"|| previousState == "Entropy"|| previousState == "Curl Magnitude") {
        pa->changeFile(ui->horizontalSlider->value());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    if (previousState == "Gravitational Potential") {
        volumeMapper->SetInputData(getData(ui->horizontalSlider->value(), "gravPo"));
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    if (previousState == "Divergence") {
        volumeMapper->SetInputData(getData(ui->horizontalSlider->value(), "divergence"));
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
};

void MainWindow::setFileId(int num) {
    fileId = num;
};

void MainWindow::setRenderWindow(vtkSmartPointer<vtkRenderer> ren) {
    renderWindow->AddRenderer(ren);
    this->ui->openGLWidget->SetRenderWindow(renderWindow);
};

void MainWindow::visualize(std::string feature,vtkSmartPointer<vtkImageData> imageData) {
    // Create transfer mapping scalar value to opacity
    vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
    if (feature == "gravPo") {
        opacityTransferFunction->AddPoint(-50, 0.1);
        opacityTransferFunction->AddPoint(0, 0);
        opacityTransferFunction->AddPoint(50, 0.1);
        opacityTransferFunction->AddPoint(2000, 0.1);
        opacityTransferFunction->AddPoint(3000, 1);
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
    if (feature == "gravPo") {
        colorTransferFunction->AddRGBPoint(-100.0, 0, 1, 0);
        colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);
        colorTransferFunction->AddRGBPoint(30, 1, 0, 0);
        colorTransferFunction->AddRGBPoint(80, 1, 0, 0);
        colorTransferFunction->AddRGBPoint(2000, 1, 0, 0);
        colorTransferFunction->AddRGBPoint(3000, 1, 1, 1);
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
    
    volumeMapper= vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
    volumeMapper->SetInputData(imageData);

    vtkNew<vtkVolume> volume;
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    vtkNew<vtkRenderer> ren1;
    ren1->AddVolume(volume);
    ren1->SetBackground(0.0, 0.0, 0.0);
    ren1->GetActiveCamera()->Azimuth(25);
    ren1->ResetCameraClippingRange();
    ren1->ResetCamera();

    setRenderWindow(ren1);
    this->ui->openGLWidget->GetRenderWindow()->Render();
    update();
};

vtkSmartPointer<vtkImageData> MainWindow::getData(int fileId, std::string feature) {
    std::string root_path = "C:/Visualization-21/data";
    std::string filepath = root_path + "/sph_result_" + std::to_string(fileId) + "_" + feature + ".txt";
    std::ifstream input(filepath);
    int gridsize;
    input >> gridsize;
    double corner1x, corner1y, corner1z;
    input >> corner1x >> corner1y >> corner1z;
    double corner2x, corner2y, corner2z;
    input >> corner2x >> corner2y >> corner2z;
    std::vector<double> value;
    double t;
    double maxt = 0;
    for (int i = 0; i < gridsize * gridsize * gridsize; i++) {
        input >> t;
        value.push_back(t);
        maxt = std::max(maxt, t);
    }

    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
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
                if (pa->boundPoint(gridsize, x, y, z) && feature == "divergence") {
                    *ptr++ = 1000000;
                }
                else if (pa->boundPoint(gridsize, x, y, z) && feature == "gravPo") {
                    *ptr++ = 10000;
                }
                else {
                    *ptr++ = value[idx];
                }
            }
        }
    }
    return imageData;
};