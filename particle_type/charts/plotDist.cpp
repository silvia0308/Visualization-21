#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkNew.h>
#include <vtkAxis.h>

using namespace std;

int main(int, char* [])
{
    std::vector<float> vid, vstar, vform, vwind, vAGN;
    ifstream input("../distribute.txt");
    string s;
    input >> s;
    float id, star, form, wind, AGN;
    for (int j = 0; j < 313; j++) {
        input >> id >> star >> form >> wind >> AGN;
        vid.push_back(id);
        vstar.push_back(star);
        vform.push_back(form);
        vwind.push_back(wind);
        vAGN.push_back(AGN);
    }
    input.close();

    // Create a table with some points in it
    vtkNew<vtkTable> table;

    vtkNew<vtkFloatArray> arr0;
    arr0->SetName("Time Step");
    table->AddColumn(arr0);

    vtkNew<vtkFloatArray> arr1;
    arr1->SetName("Star Particles");
    table->AddColumn(arr1);

    vtkNew<vtkFloatArray> arr2;
    arr2->SetName("Star Forming Particles");
    table->AddColumn(arr2);

    vtkNew<vtkFloatArray> arr3;
    arr3->SetName("AGN Particles");
    table->AddColumn(arr3);

    vtkNew<vtkFloatArray> arr4;
    arr4->SetName("Wind Particles");
    table->AddColumn(arr4);

    // Fill in the table with some example values
    int numPoints = 313;
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
        table->SetValue(i, 0, vid[i]);
        table->SetValue(i, 1, vstar[i] + 0.1);
        table->SetValue(i, 2, vform[i] + 0.1);
        table->SetValue(i, 3, vAGN[i] + 0.1);
        table->SetValue(i, 4, vwind[i] + 0.1);
    }

    // Set up the view
    vtkNew<vtkContextView> view;
    view->GetRenderWindow()->SetWindowName("Plot");
    view->GetRenderWindow()->SetSize(800, 800);
    view->GetRenderer()->SetBackground(1, 1, 1);

    // Add multiple line plots, setting the colors etc
    vtkNew<vtkChartXY> chart;
    view->GetScene()->AddItem(chart);
    vtkPlot* line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    line->SetColor(255, 0, 0, 255);
    line->SetWidth(2.0);
    line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 2);
    line->SetColor(255, 165, 0, 255);
    line->SetWidth(2.0);
    line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 3);
    line->SetColor(128, 0, 255, 128);
    line->SetWidth(2.0);
    line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 4);
    line->SetColor(0, 100, 0, 255);
    line->SetWidth(2.0);

    auto XAxis = line->GetXAxis();
    XAxis->SetTitle("Time Step");

    auto YAxis = line->GetYAxis();
    YAxis->SetMinimumLimit(0.5);
    YAxis->SetLogScale(true);
    YAxis->SetTitle("Number of Particles");

    //line->SetLegendVisibility(true);
    line->Update();

    // Start interactor
    view->GetRenderWindow()->Render();
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}
