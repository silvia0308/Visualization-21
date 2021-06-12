#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vtkGenericOpenGLRenderWindow.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

// Constructor
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), pt(new ParticleType()),pa(new ParticleAttributes()),cw(new CosWeb())
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
    std::cout << ui->horizontalSlider->value() << std::endl;
    //Cosmic Web
    if (previousState != ui->checkBox->text().toStdString() && ui->checkBox->isChecked()) {
        std::cout << ui->checkBox->text().toStdString() << std::endl;
        previousState = ui->checkBox->text().toStdString();
        cw->getData(ui->horizontalSlider->value());
        cw->setMapper();
        setRenderWindow(cw->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Particle Type
    if (previousState != ui->checkBox_2->text().toStdString() && ui->checkBox_2->isChecked()) {
        std::cout << ui->checkBox_2->text().toStdString() << std::endl;
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
        std::cout << ui->radioButton->text().toStdString() << std::endl;
        previousState = ui->radioButton->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "interEn");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Density
    if (previousState != ui->radioButton_2->text().toStdString() && ui->radioButton_2->isChecked()) {
        std::cout << ui->radioButton_2->text().toStdString() << std::endl;
        previousState = ui->radioButton_2->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "density");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Gravitational Potential
    if (previousState != ui->radioButton_3->text().toStdString() && ui->radioButton_3->isChecked()) {
        std::cout << ui->radioButton_3->text().toStdString() << std::endl;
        previousState = ui->radioButton_3->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "gravPo");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Temperature
    if (previousState != ui->radioButton_4->text().toStdString() && ui->radioButton_4->isChecked()) {
        std::cout << ui->radioButton_4->text().toStdString() << std::endl;
        previousState = ui->radioButton_4->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "temperature");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Entropy
    if (previousState != ui->radioButton_5->text().toStdString() && ui->radioButton_5->isChecked()) {
        std::cout << ui->radioButton_5->text().toStdString() << std::endl;
        previousState = ui->radioButton_5->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "entropy");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Divergence
    if (previousState != ui->radioButton_6->text().toStdString() && ui->radioButton_6->isChecked()) {
        std::cout << ui->radioButton_6->text().toStdString() << std::endl;
        previousState = ui->radioButton_6->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "divergence");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
    //Curl Magnitude
    if (previousState != ui->radioButton_7->text().toStdString() && ui->radioButton_7->isChecked()) {
        std::cout << ui->radioButton_7->text().toStdString() << std::endl;
        previousState = ui->radioButton_7->text().toStdString();
        pa->setAll(ui->horizontalSlider->value(), "curlnorm");
        setRenderWindow(pa->getRenderer());
        this->ui->openGLWidget->GetRenderWindow()->Render();
        update();
    }
};

void MainWindow::on_timeChanged() {
    std::cout<<ui->horizontalSlider->value()<<std::endl;
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
    if (previousState == "Internal Energy"|| previousState == "Density"|| previousState == "Gravitational Potential"|| previousState == "Temperature"|| previousState == "Entropy"|| previousState == "Divergence"|| previousState == "Curl Magnitude") {
        pa->changeFile(ui->horizontalSlider->value());
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
