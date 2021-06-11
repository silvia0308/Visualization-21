#include "core/sph_interpolator.h"
#include "core/cosmic_web.h"
#include <utils/interp_utils.h>

namespace vis21 {

const double eps = 1e-12;

float SPHInterpolator::get_w(const Eigen::Vector3d& vec, const double& radius) const {
    if (options_.kernel == K_CUBICSPLINE)
        return getw(vec / radius);
    else // kernel == K_WC4
        return getw_wc4(vec / radius, radius);
}
Eigen::Vector3d SPHInterpolator::get_dw(const Eigen::Vector3d& vec, const double& radius) const {
    if (options_.kernel == K_CUBICSPLINE)
        return getdw(vec / radius);
    else // kernel == K_WC4
        return getdw_wc4(vec / radius, radius);
}
Eigen::Matrix3d SPHInterpolator::get_d2w(const Eigen::Vector3d& vec, const double& radius) const {
    if (options_.kernel == K_CUBICSPLINE)
        return getd2w(vec / radius);
    else // kernel == K_WC4
        return getd2w_wc4(vec / radius, radius);
}

void SPHInterpolator::initialize() {
    // initialize kd tree
    int n_points = data_->particles.size();
    points_.resize(n_points, 3);
    for (int i = 0; i < n_points; ++i) {
        points_.row(i) = data_->particles[i].p;
    }
    tree_.initialize(points_);

    // initial smoothing radii
    points_radii_.resize(n_points);
#pragma omp parallel for
    for (int i = 0; i < n_points; ++i) {
        points_radii_(i) = data_->particles[i].get_attr(ParticleType::HH);
    }
    max_point_radius_ = points_radii_.maxCoeff();

    // construct grid points
    constructGridPoints();
}

SPHInterpolator::SPHInterpolator(const std::shared_ptr<DataFrame>& pd, const SPHOptions& options) {
    data_ = pd; options_ = options;
    initialize();
}

void SPHInterpolator::constructGridPoints() {
    // construct grid points and update wendland radius
    int resolution = options_.resolution;
    int n_grid_points = resolution * resolution * resolution;
    grid_points_.resize(n_grid_points, 3);
    val_array_.resize(n_grid_points);

    Eigen::RowVector3d bb_min, bb_max;
    bb_min = points_.colwise().minCoeff();
    bb_max = points_.colwise().maxCoeff();
    Eigen::RowVector3d minmax_dist = bb_max - bb_min;
    bb_min = bb_min - ((options_.enlarge_ratio - 1) / 2.0) * minmax_dist;
    bb_max = bb_max + ((options_.enlarge_ratio - 1) / 2.0) * minmax_dist;
    Eigen::RowVector3d dim = bb_max - bb_min;

    double dx, dy, dz;
    dx = dim[0] / (resolution - 1); step_x_ = dx;
    dy = dim[1] / (resolution - 1); step_y_ = dy;
    dz = dim[2] / (resolution - 1); step_z_ = dz;
    double scale = std::max(dx, std::max(dy, dz));
    w_radius = options_.w_radius_ratio * scale;
    for (int x = 0; x < options_.resolution; ++x) {
        for (int y = 0; y < options_.resolution; ++y) {
            for (int z = 0; z < options_.resolution; ++z) {
                int id = (x * resolution + y) * resolution + z;
                grid_points_(id, 0) = bb_min[0] + x * dx;
                grid_points_(id, 1) = bb_min[1] + y * dy;
                grid_points_(id, 2) = bb_min[2] + z * dz;
            }
        }
    }
    if (options_.use_speedup)
        tree_grids_.initialize(grid_points_);
}

void SPHInterpolator::setGridPoints(const Eigen::MatrixXd &points) {
    // update grid points
    grid_points_ = points;
    if (options_.use_speedup)
        tree_grids_.initialize(grid_points_);

    // update wendland radius
    Eigen::RowVector3d bb_min, bb_max;
    bb_min = points_.colwise().minCoeff();
    bb_max = points_.colwise().maxCoeff();
    Eigen::RowVector3d dim = bb_max - bb_min;
    double dx, dy, dz;
    int resolution = options_.resolution;
    dx = dim[0] / (resolution - 1); step_x_ = dx;
    dy = dim[1] / (resolution - 1); step_y_ = dy;
    dz = dim[2] / (resolution - 1); step_z_ = dz;
    double scale = std::max(dx, std::max(dy, dz));
    w_radius = options_.w_radius_ratio * scale;
}

void SPHInterpolator::computeDensityArrayOriginal() {
    // compute density array
    density_array_.setZero(points_.rows());
#pragma omp parallel for
    for (int i = 0; i < points_.rows(); ++i) {
        Eigen::Vector3d p = points_.row(i);
        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_.query_radius_search(p, return_idx, max_point_radius_);
        else
            tree_.query_radius_search(p, return_idx, w_radius);
        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = points_.row(idx);
            Eigen::Vector3d dist = p - q;
            float w;
            if (options_.use_hh) {
                if (dist.norm() > points_radii_[idx])
                    continue;
                w = get_w(dist, points_radii_[idx] + eps);
            }
            else {
                if (dist.norm() > points_radii_[idx] && options_.use_hh_advanced)
                    continue;
                w = get_w(dist, w_radius);
            }
            float mass = data_->particles[idx].get_mass();
            float val = mass * w;
            density_array_[i] = density_array_[i] + val;
        }
    }
}

void SPHInterpolator::interpolate_val_original(const ParticleType::AttributeType &type) {
    // compute sph density array
    computeDensityArray();

    val_array_.setZero(grid_points_.rows());
#pragma omp parallel for
    for (int i = 0; i < grid_points_.rows(); ++i) {
        Eigen::Vector3d p = grid_points_.row(i);
        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_.query_radius_search(p, return_idx, max_point_radius_);
        else
            tree_.query_radius_search(p, return_idx, w_radius);
        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = points_.row(idx);
            Eigen::Vector3d dist = p - q;
            float w;
            if (options_.use_hh) {
                if (dist.norm() > points_radii_[idx])
                    continue;
                w = get_w(dist, points_radii_[idx] + eps);
            }
            else {
                if (dist.norm() > points_radii_[idx] && options_.use_hh_advanced)
                    continue;
                w = get_w(dist, w_radius);
            }
            float mass = data_->particles[idx].get_mass();
            float rho = density_array_[idx];
            float val = (mass / rho) * data_->particles[idx].get_attr(type) * w;
            val_array_(i) = val_array_(i) + val;
        }
    }
}

void SPHInterpolator::interpolate_grad_original(const ParticleType::AttributeType &type) {
    // compute sph density array
    computeDensityArray();

    grad_array_.resize(grid_points_.rows());
#pragma omp parallel for
    for (int i = 0; i < grid_points_.rows(); ++i) {
        grad_array_[i].setZero();
        Eigen::Vector3d p = grid_points_.row(i);
        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_.query_radius_search(p, return_idx, max_point_radius_);
        else
            tree_.query_radius_search(p, return_idx, w_radius);
        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = points_.row(idx);
            Eigen::Vector3d dist = p - q;
            Eigen::Vector3d dw;
            if (options_.use_hh) {
                if (dist.norm() > points_radii_[idx])
                    continue;
                dw = get_dw(dist, points_radii_[idx] + eps);
            }
            else {
                if (dist.norm() > points_radii_[idx] && options_.use_hh_advanced)
                    continue;
                dw = get_dw(dist, w_radius);
            }
            float mass = data_->particles[idx].get_mass();
            float rho = density_array_[idx];
            Eigen::Vector3d grad = (mass / rho) * data_->particles[idx].get_attr(type) * dw;
            grad_array_[i] += grad;
        }
    }
}

void SPHInterpolator::interpolate_hessian_original(const ParticleType::AttributeType &type) {
    // compute sph density array
    computeDensityArray();

    hessian_array_.resize(grid_points_.rows());
#pragma omp parallel for
    for (int i = 0; i < grid_points_.rows(); ++i) {
        hessian_array_[i].setZero();
        Eigen::Vector3d p = grid_points_.row(i);
        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_.query_radius_search(p, return_idx, max_point_radius_);
        else
            tree_.query_radius_search(p, return_idx, w_radius);
        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = points_.row(idx);
            Eigen::Vector3d dist = p - q;
            Eigen::Matrix3d d2w;
            if (options_.use_hh) {
                if (dist.norm() > points_radii_[idx])
                    continue;
                d2w = get_d2w(dist, points_radii_[idx] + eps);
            }
            else {
                if (dist.norm() > points_radii_[idx] && options_.use_hh_advanced)
                    continue;
                d2w = get_d2w(dist, w_radius);
            }
            float mass = data_->particles[idx].get_mass();
            float rho = density_array_[idx];
            Eigen::Matrix3d hessian = (mass / rho) * data_->particles[idx].get_attr(type) * d2w;
            hessian_array_[i] += hessian;
        }
    }
}

void SPHInterpolator::computeDensityArraySpeedup() {
    density_array_.setZero(points_.rows());
    for (int i = 0; i < points_.rows(); ++i) {
        Eigen::Vector3d p = points_.row(i);
        float mass = data_->particles[i].get_mass();

        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_.query_radius_search(p, return_idx, points_radii_(i));
        else
            tree_.query_radius_search(p, return_idx, w_radius);

        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = points_.row(idx);
            Eigen::Vector3d dist = q - p;
            float w;
            if (options_.use_hh)
                w = get_w(dist, points_radii_(i) + eps);
            else
                w = get_w(dist, w_radius);
            float val = mass * w;
            density_array_[idx] = density_array_[idx] + val;
        }
    }
}

void SPHInterpolator::interpolate_val_speedup(const ParticleType::AttributeType &type) {
    // compute sph density array
    computeDensityArraySpeedup();

    val_array_.setZero(grid_points_.rows());
    for (int i = 0; i < points_.rows(); ++i) {
        Eigen::Vector3d p = grid_points_.row(i);
        float mass = data_->particles[i].get_mass();
        float rho = density_array_[i];
        float val_i = data_->particles[i].get_attr(type);

        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_grids_.query_radius_search(p, return_idx, points_radii_(i));
        else
            tree_grids_.query_radius_search(p, return_idx, w_radius);

        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = grid_points_.row(idx);
            Eigen::Vector3d dist = q - p;
            float w;
            if (options_.use_hh)
                w = get_w(dist, points_radii_(i) + eps);
            else
                w = get_w(dist, w_radius);
            float val = (mass / rho) * val_i * w;
            val_array_(idx) = val_array_(idx) + val;
        }
    }
}

void SPHInterpolator::interpolate_grad_speedup(const ParticleType::AttributeType &type) {
    // compute sph density array
    computeDensityArraySpeedup();

    grad_array_.resize(grid_points_.rows());
    for (int i = 0; i < points_.rows(); ++i) {
        Eigen::Vector3d p = grid_points_.row(i);
        float mass = data_->particles[i].get_mass();
        float rho = density_array_[i];
        float val_i = data_->particles[i].get_attr(type);

        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_grids_.query_radius_search(p, return_idx, points_radii_(i));
        else
            tree_grids_.query_radius_search(p, return_idx, w_radius);

        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = grid_points_.row(idx);
            Eigen::Vector3d dist = q - p;
            Eigen::Vector3d dw;
            if (options_.use_hh)
                dw = get_dw(dist, points_radii_(i) + eps);
            else
                dw = get_dw(dist, w_radius);
            Eigen::Vector3d val = (mass / rho) * val_i * dw;
            grad_array_[idx] += val;
        }
    }
}

void SPHInterpolator::interpolate_hessian_speedup(const ParticleType::AttributeType &type) {
    // compute sph density array
    computeDensityArraySpeedup();

    hessian_array_.resize(grid_points_.rows());
    for (int i = 0; i < points_.rows(); ++i) {
        Eigen::Vector3d p = grid_points_.row(i);
        float mass = data_->particles[i].get_mass();
        float rho = density_array_[i];
        float val_i = data_->particles[i].get_attr(type);

        std::vector<int> return_idx;
        if (options_.use_hh) 
            tree_grids_.query_radius_search(p, return_idx, points_radii_(i));
        else
            tree_grids_.query_radius_search(p, return_idx, w_radius);

        for (int j = 0; j < return_idx.size(); ++j) {
            int idx = return_idx[j];
            Eigen::Vector3d q = grid_points_.row(idx);
            Eigen::Vector3d dist = q - p;
            Eigen::Matrix3d d2w;
            if (options_.use_hh)
                d2w = get_d2w(dist, points_radii_(i) + eps);
            else
                d2w = get_d2w(dist, w_radius);
            Eigen::Matrix3d val = (mass / rho) * val_i * d2w;
            hessian_array_[idx] += val;
        }
    }
}

void SPHInterpolator::computeDensityArray() {
    if (options_.use_speedup)
        computeDensityArraySpeedup();
    else
        computeDensityArrayOriginal();
}

void SPHInterpolator::interpolate_val(const ParticleType::AttributeType &type) {
    if (options_.use_speedup)
        interpolate_val_speedup(type);
    else
        interpolate_val_original(type);
}

void SPHInterpolator::interpolate_grad(const ParticleType::AttributeType &type) {
    if (options_.use_speedup)
        interpolate_grad_speedup(type);
    else
        interpolate_grad_original(type);
}


void SPHInterpolator::interpolate_hessian(const ParticleType::AttributeType &type) {
    if (options_.use_speedup)
        interpolate_hessian_speedup(type);
    else
        interpolate_hessian_original(type);
}

void SPHInterpolator::Run(const ParticleType::AttributeType &type) {
    interpolate_val(type);
}

void SPHInterpolator::RunDivandCurl() {
    interpolate_grad(ParticleType::VX);
    std::vector<Eigen::Vector3d> u_grad = grad_array_;
    interpolate_grad(ParticleType::VY);
    std::vector<Eigen::Vector3d> v_grad = grad_array_;
    interpolate_grad(ParticleType::VX);
    std::vector<Eigen::Vector3d> w_grad = grad_array_;

    // update div_array_
    div_array_.setZero(grid_points_.rows());
#pragma omp parallel for
    for (int i = 0; i < grid_points_.rows(); ++i) {
        div_array_(i) += u_grad[i](0);
        div_array_(i) += v_grad[i](1);
        div_array_(i) += w_grad[i](2);
    }
    // update curl_norm_array_
    curl_norm_array_.setZero(grid_points_.rows());
#pragma omp parallel for
    for (int i = 0; i < grid_points_.rows(); ++i) {
        Eigen::Vector3d vec;
        vec(0) = w_grad[i](1) - v_grad[i](2);
        vec(1) = u_grad[i](2) - w_grad[i](0);
        vec(2) = v_grad[i](0) - u_grad[i](1);
        curl_norm_array_[i] = vec.norm();
    }
}

int SPHInterpolator::get_gridid(const int& x, const int& y, const int& z) {
    return (x * options_.resolution + y) * options_.resolution + z;
}

void SPHInterpolator::interpolate_grad(const Eigen::VectorXd& val_array) {
    int n_grid_points = val_array.rows();
    grad_array_.resize(n_grid_points);
#pragma omp parallel for
    for (int idx = 0; idx < val_array.rows(); ++idx) {
        int temp = idx / options_.resolution;
        int x = temp / options_.resolution;
        int y = temp - x * options_.resolution;
        int z = idx - temp * options_.resolution;
        
        // compute dx
        double dx;
        if (x == 0)
            dx = (1 / step_x_) * (val_array(get_gridid(x+1, y, z)) - val_array(idx));
        else if (x == options_.resolution - 1)
            dx = (1 / step_x_) * (val_array(idx) - val_array(get_gridid(x-1, y, z)));
        else
            dx = (2 / step_x_) * (val_array(get_gridid(x+1, y, z)) - val_array(get_gridid(x-1, y, z)));

        // compute dy
        double dy;
        if (y == 0)
            dy = (1 / step_y_) * (val_array(get_gridid(x, y+1, z)) - val_array(idx));
        else if (y == options_.resolution - 1)
            dy = (1 / step_y_) * (val_array(idx) - val_array(get_gridid(x, y-1, z)));
        else
            dy = (2 / step_y_) * (val_array(get_gridid(x, y+1, z)) - val_array(get_gridid(x, y-1, z)));

        // compute dz
        double dz;
        if (z == 0)
            dz = (1 / step_z_) * (val_array(get_gridid(x, y, z+1)) - val_array(idx));
        else if (z == options_.resolution - 1)
            dz = (1 / step_z_) * (val_array(idx) - val_array(get_gridid(x, y, z-1)));
        else
            dz = (2 / step_z_) * (val_array(get_gridid(x, y, z+1)) - val_array(get_gridid(x, y, z-1)));

        // set grad array
        grad_array_[idx] = Eigen::Vector3d(dx, dy, dz);
    }
}

void SPHInterpolator::interpolate_hessian(const std::vector<Eigen::Vector3d>& grad_array) {
    int n_grid_points = grad_array.size();
    hessian_array_.resize(n_grid_points);
#pragma omp parallel for
    for (int idx = 0; idx < grad_array.size(); ++idx) {
        int temp = idx / options_.resolution;
        int x = temp / options_.resolution;
        int y = temp - x * options_.resolution;
        int z = idx - temp * options_.resolution;
        
        // compute dx
        Eigen::Vector3d dx;
        if (x == 0)
            dx = (1 / step_x_) * (grad_array[get_gridid(x+1, y, z)] - grad_array[idx]);
        else if (x == options_.resolution - 1)
            dx = (1 / step_x_) * (grad_array[idx] - grad_array[get_gridid(x-1, y, z)]);
        else
            dx = (2 / step_x_) * (grad_array[get_gridid(x+1, y, z)] - grad_array[get_gridid(x-1, y, z)]);

        // compute dy
        Eigen::Vector3d dy;
        if (y == 0)
            dy = (1 / step_y_) * (grad_array[get_gridid(x, y+1, z)] - grad_array[idx]);
        else if (y == options_.resolution - 1)
            dy = (1 / step_y_) * (grad_array[idx] - grad_array[get_gridid(x, y-1, z)]);
        else
            dy = (2 / step_y_) * (grad_array[get_gridid(x, y+1, z)] - grad_array[get_gridid(x, y-1, z)]);

        // compute dz
        Eigen::Vector3d dz;
        if (z == 0)
            dz = (1 / step_z_) * (grad_array[get_gridid(x, y, z+1)] - grad_array[idx]);
        else if (z == options_.resolution - 1)
            dz = (1 / step_z_) * (grad_array[idx] - grad_array[get_gridid(x, y, z-1)]);
        else
            dz = (2 / step_z_) * (grad_array[get_gridid(x, y, z+1)] - grad_array[get_gridid(x, y, z-1)]);

        // set grad array
        hessian_array_[idx].row(0) = dx;
        hessian_array_[idx].row(1) = dy;
        hessian_array_[idx].row(2) = dz;
    }
}

void SPHInterpolator::ComputeCosmicweb(std::vector<Eigen::Vector3d>& points, const ParticleType::AttributeType type) {
    // compute sph density array
    computeDensityArray();

    // compute gradient, hessian and hessian times gradient
    if (options_.use_finite_difference) {
        interpolate_val(type);
        interpolate_grad(val_array_);
        interpolate_hessian(grad_array_);
    }
    else {
        interpolate_grad(type);
        interpolate_hessian(type);
    }
    hesxgrad_array_.resize(grid_points_.rows());
#pragma omp parallel for
    for (int i = 0; i < grid_points_.rows(); ++i) {
        hesxgrad_array_[i] = hessian_array_[i] * grad_array_[i];
    }

    // cosmic web G.cross(HG) = 0
    CosmicwebOptions cw_options;
    cw_options.eps_null = 1e-6;
    cw_options.eps_duplicate = 1e-6;
    cw_options.eps_refine = 1e-8;
    CosmicwebEngine cw_engine(cw_options, grid_points_, grad_array_, hesxgrad_array_);
    cw_engine.Run(points);
    if (options_.do_print)
        std::cout<<"Cosmic web completed (n_points = "<<points.size()<<")"<<std::endl;
}

} // namespace vis21

