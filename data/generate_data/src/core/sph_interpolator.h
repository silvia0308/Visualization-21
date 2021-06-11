#ifndef VIS21_SRC_CORE_SPHINTERPOLATOR_H
#define VIS21_SRC_CORE_SPHINTERPOLATOR_H

#include <utils/kd_tree.h>
#include <vector>
#include "core/dataframe.h"

namespace vis21 {

enum SPHKernel {
    K_CUBICSPLINE=0,
    K_WC4=1
};

struct SPHOptions {
    bool do_print = false;
    int resolution = 101;
    double enlarge_ratio = 1.1;

    SPHKernel kernel = K_WC4;
    double w_radius_ratio = 2.0; // the ratio of w_radius over [dx, dy, dz].max()
    bool use_hh = false; // directly use provided smoothing radii
    bool use_hh_advanced = true; // use provided smoothing radii as an additional test
    bool use_speedup = false; // without use openmp by using grid kdtree
    bool use_finite_difference = true; // use finite difference for grad and hessian computation for cosmic web
};

class SPHInterpolator {
private:
    SPHOptions options_;
    std::shared_ptr<DataFrame> data_;
    KDTree tree_;
    KDTree tree_grids_;
    float w_radius; // initialized using w_radius_ratio
    double step_x_, step_y_, step_z_;

    Eigen::MatrixXd points_;
    Eigen::VectorXd points_radii_; // store the smoothing radius for each point
    double max_point_radius_;
    Eigen::VectorXd density_array_; // per-point density for sph computation, same size as points_.rows()

    Eigen::MatrixXd grid_points_;
    Eigen::VectorXd val_array_; // sph result, same size as grid_points_.rows()
    Eigen::VectorXd div_array_; // divergence result, same size as grid_points_.rows()
    Eigen::VectorXd curl_norm_array_; // curl norm result, same size as grid_points_.rows()
    std::vector<Eigen::Vector3d> grad_array_; // gradient result
    std::vector<Eigen::Matrix3d> hessian_array_; // hessian result
    std::vector<Eigen::Vector3d> hesxgrad_array_; // hessian times gradient

    void initialize();

    float get_w(const Eigen::Vector3d& vec, const double& radius) const;
    Eigen::Vector3d get_dw(const Eigen::Vector3d& vec, const double& radius) const;
    Eigen::Matrix3d get_d2w(const Eigen::Vector3d& vec, const double& radius) const;

    // sph interpolation functions
    // with openmp grid query point kdtree
    void computeDensityArrayOriginal();
    void interpolate_val_original(const ParticleType::AttributeType& type); // stored in val_array_
    void interpolate_grad_original(const ParticleType::AttributeType& type); // stored in grad_array_
    void interpolate_hessian_original(const ParticleType::AttributeType& type); // stored in hessian_array_

    // without openmp point query grid kdtree
    void computeDensityArraySpeedup();
    void interpolate_val_speedup(const ParticleType::AttributeType& type); // stored in val_array_
    void interpolate_grad_speedup(const ParticleType::AttributeType& type); // stored in grad_array_
    void interpolate_hessian_speedup(const ParticleType::AttributeType& type); // stored in hessian_array_

    void computeDensityArray();
    void interpolate_val(const ParticleType::AttributeType& type); // stored in val_array_
    void interpolate_grad(const ParticleType::AttributeType& type); // stored in grad_array_
    void interpolate_hessian(const ParticleType::AttributeType& type); // stored in hessian_array_

    int get_gridid(const int& x, const int& y, const int& z);
    void interpolate_grad(const Eigen::VectorXd& val_array); // fd version, stored in grad_array_
    void interpolate_hessian(const std::vector<Eigen::Vector3d>& grad_array); // fd version, stored in hessian_array_

public:
    SPHInterpolator() {};
    ~SPHInterpolator() {};
    SPHInterpolator(const std::shared_ptr<DataFrame>& pd, const SPHOptions& options);

    Eigen::MatrixXd getGridPoints() {return grid_points_;}
    Eigen::VectorXd getResult() {return val_array_;}
    Eigen::VectorXd getDivergence() {return div_array_;}
    Eigen::VectorXd getCurlNorm() {return curl_norm_array_;}
    void constructGridPoints();
    void setGridPoints(const Eigen::MatrixXd& points);


    void Run(const ParticleType::AttributeType& type);
    void RunDivandCurl();
    void ComputeCosmicweb(std::vector<Eigen::Vector3d>& points, const ParticleType::AttributeType type=ParticleType::RHO);
};

} // namespace vis21

#endif // VIS21_SRC_CORE_SPHINTERPOLATOR_H

