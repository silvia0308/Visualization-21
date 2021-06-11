#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <core/dataframe.h>
#include <core/sph_interpolator.h>

void readPointsFromFile(const std::string& filename, std::vector<Eigen::Vector3d>& points) {
    std::ifstream myfile;
    myfile.open(filename);

    int n_points; myfile >> n_points;
    points.resize(n_points);
    for (int i = 0; i < n_points; ++i) {
        double x, y, z; myfile >> x >> y >> z; points[i] = Eigen::Vector3d(x, y, z);
    }
}

void writePointsToFile(const std::string& filename, const std::vector<Eigen::Vector3d>& points) {
    std::ofstream myfile;
    myfile.open(filename);

    myfile << points.size() << "\n";
    for (int i = 0; i < points.size(); ++i) {
        myfile << points[i](0) << " " << points[i](1) << " " << points[i](2) << "\n";
    }
}

void run() {
    std::string root_path = "/media/shaoliu/data/Cosmology/";
    int total_file = 313;
    Eigen::MatrixXd grid_points; // We share a same grid across different data frames
    int start_num = 200;
    for (int f = 0; f < total_file; ++f) {
        if (f < start_num && f != 0)
            continue;
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

        // load dataset
        std::shared_ptr<vis21::DataFrame> pd = std::shared_ptr<vis21::DataFrame>(new vis21::DataFrame(file_path, f * 2));

        // sph starts here
        vis21::SPHOptions sph_options;
        sph_options.do_print = true;
        sph_options.resolution = 101;
        sph_options.w_radius_ratio = 8.0;
        vis21::SPHInterpolator sph_engine(pd, sph_options);
        if (f == 0)
            grid_points = sph_engine.getGridPoints();
        else
            sph_engine.setGridPoints(grid_points);
        if (f < start_num)
            continue;

        std::vector<Eigen::Vector3d> points;
        sph_engine.ComputeCosmicweb(points, vis21::ParticleType::RHO);
        std::string filename = "cosmicweb_result_" + std::to_string(f) + ".txt";
        writePointsToFile(filename, points);

        std::cout<<"[COSMICWEB] dataframe ("<<f<<" / "<<total_file<<") completed."<<std::endl;
    }
}

int main() {
    run();
}
