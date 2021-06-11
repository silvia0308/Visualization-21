#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <core/dataframe.h>
#include <core/sph_interpolator.h>

void readSPHResultFromFile(const std::string& filename, Eigen::MatrixXd &grid_points, Eigen::VectorXd &val_array) {
    std::ifstream myfile;
    myfile.open(filename);

    // read grid_grid_points
    int resolution; myfile >> resolution;
    Eigen::RowVector3d bb_min, bb_max;
    myfile >> bb_min[0] >> bb_min[1] >> bb_min[2];
    myfile >> bb_max[0] >> bb_max[1] >> bb_max[2];
    int n_grid_points = resolution * resolution * resolution;
    grid_points.resize(n_grid_points, 3);
    Eigen::RowVector3d dim = bb_max - bb_min;
    double dx, dy, dz; 
    dx = dim[0] / (resolution - 1);
    dy = dim[1] / (resolution - 1);
    dz = dim[2] / (resolution - 1);
    for (int x = 0; x < resolution; ++x) {
        for (int y = 0; y < resolution; ++y) {
            for (int z = 0; z < resolution; ++z) {
                int id = (x * resolution + y) * resolution + z;
                grid_points(id, 0) = bb_min[0] + x * dx;
                grid_points(id, 1) = bb_min[1] + y * dy;
                grid_points(id, 2) = bb_min[2] + z * dz;
            }
        }
    }

    // read val_array
    val_array.resize(n_grid_points);
    for (int i = 0; i < n_grid_points; ++i) {
        double k; myfile >> k; val_array(i) = k;
    }
}

void writeSPHResultToFile(const std::string& filename, const Eigen::MatrixXd grid_points, const Eigen::VectorXd& val_array) {
    std::ofstream myfile;
    myfile.open(filename);

    // save resolution and bb_min, bb_max
    int resolution = std::round(pow(grid_points.rows(), 1.0 / 3.0));
    Eigen::RowVector3d bb_min, bb_max;
    bb_min = grid_points.colwise().minCoeff();
    bb_max = grid_points.colwise().maxCoeff();
    myfile << resolution << "\n";
    myfile << bb_min[0] << " " << bb_min[1] << " " << bb_min[2] << std::endl;
    myfile << bb_max[0] << " " << bb_max[1] << " " << bb_max[2] << std::endl;

    // save val_array
    for (int i = 0; i < grid_points.rows(); ++i) {
        myfile << val_array(i) << "\n";
    }
    std::cout<<"Writing "<<filename<<" completed."<<std::endl;
}

void run() {
    std::string root_path;
    std::cout << "Please input the path to cosmology folder: ";
    std::cin >> root_path;
    int total_file = 313;
    Eigen::MatrixXd grid_points; // We share a same grid across different data frames

    //int start_num = 160;
    for (int f = 0; f < total_file; ++f) {
        /*if (f <= start_num && f != 0) // We need the first frame to get initial grid
            continue;*/
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
        std::string file_path = root_path + "/Full.cosmo." + file_name + ".vtp"; 

        // load dataset
        std::shared_ptr<vis21::DataFrame> pd = std::shared_ptr<vis21::DataFrame>(new vis21::DataFrame(file_path, f * 2));

        // sph starts here
        vis21::SPHOptions sph_options;
        sph_options.use_hh = false;
        sph_options.resolution = 200;
        sph_options.w_radius_ratio = 2.0;
        vis21::SPHInterpolator sph_engine(pd, sph_options);
        if (f == 0)
            grid_points = sph_engine.getGridPoints();
        else
            sph_engine.setGridPoints(grid_points);
        /*if (f <= start_num)
            continue;*/

        // SPH over internal energy
        sph_engine.Run(vis21::ParticleType::UU);
        Eigen::VectorXd grid_val_interEn = sph_engine.getResult();
        std::string filename_1 = "sph_result_" + std::to_string(f) + "_interEn.txt";
        writeSPHResultToFile(filename_1, grid_points, grid_val_interEn);

        // SPH over density
        sph_engine.Run(vis21::ParticleType::RHO);
        Eigen::VectorXd grid_val_density = sph_engine.getResult();
        std::string filename_2 = "sph_result_" + std::to_string(f) + "_density.txt";
        writeSPHResultToFile(filename_2, grid_points, grid_val_density);

        // SPH over gravitational potential
        sph_engine.Run(vis21::ParticleType::PHI);
        Eigen::VectorXd grid_val_gravPo = sph_engine.getResult();
        std::string filename_3 = "sph_result_" + std::to_string(f) + "_gravPo.txt";
        writeSPHResultToFile(filename_3, grid_points, grid_val_gravPo);

        // SPH over temperature
        sph_engine.Run(vis21::ParticleType::TEMPERATURE);
        Eigen::VectorXd grid_val_temperature = sph_engine.getResult();
        std::string filename_4 = "sph_result_" + std::to_string(f) + "_temperature.txt";
        writeSPHResultToFile(filename_4, grid_points, grid_val_temperature);

        // SPH over entropy
        sph_engine.Run(vis21::ParticleType::ENTROPY);
        Eigen::VectorXd grid_val_entropy = sph_engine.getResult();
        std::string filename_5 = "sph_result_" + std::to_string(f) + "_entropy.txt";
        writeSPHResultToFile(filename_5, grid_points, grid_val_entropy);

        // SPH to get divergence and norm of curl for velocities
        sph_engine.RunDivandCurl();
        Eigen::VectorXd grid_val_divergence = sph_engine.getDivergence();
        std::string filename_6 = "sph_result_" + std::to_string(f) + "_divergence.txt";
        writeSPHResultToFile(filename_6, grid_points, grid_val_divergence);
        Eigen::VectorXd grid_val_curlnorm = sph_engine.getCurlNorm();
        std::string filename_7 = "sph_result_" + std::to_string(f) + "_curlnorm.txt";
        writeSPHResultToFile(filename_7, grid_points, grid_val_curlnorm);

        std::cout<<"[SPH] dataframe ("<<f<<" / "<<total_file<<") completed."<<std::endl;
    }
}

int main() {
    run();
}

