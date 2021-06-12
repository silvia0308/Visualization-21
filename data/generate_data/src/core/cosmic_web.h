#ifndef VIS21_SRC_CORE_COSMICWEB_ENGINE_H
#define VIS21_SRC_CORE_COSMICWEB_ENGINE_H

#include <Eigen/Core>
#include <vector>

namespace vis21 {

struct CosmicwebOptions {
    bool do_print = false;

    double eps_duplicate = 1e-6;
    double eps_null = 1e-6;
    double eps_refine = 1e-8;
    double eps_density = 1e10;
};

typedef Eigen::Vector3d Vec3d;
struct Vec3dQuad {
    Vec3d p00, p10, p01, p11;
    Vec3dQuad(const Vec3d& p00, const Vec3d& p10, const Vec3d& p01, const Vec3d& p11)
        : p00(p00), p10(p10), p01(p01), p11(p11) {}
};
struct Vec3dQuadTuple {
    Vec3dQuad x, u, v;
    Vec3dQuadTuple(const Vec3dQuad& x, const Vec3dQuad& u, const Vec3dQuad& v)
        : x(x), u(u), v(v) {}
};

class CosmicwebEngine {
private:
    CosmicwebOptions options_;

    Eigen::MatrixXd grid_points_;
    int resolution;
    Eigen::VectorXd vals_;
    std::vector<Eigen::Vector3d> grads_;
    std::vector<Eigen::Vector3d> hesxgrads_;

    bool PntContained(const Vec3d& candidate, const std::vector<Vec3d>& points);
    void SolveQuad_Bezier(const Vec3d& x00, const Vec3d& x10, const Vec3d& x01, const Vec3d& x11,
   	 const Vec3d& u00, const Vec3d& u10, const Vec3d& u01, const Vec3d& u11,
   	 const Vec3d& v00, const Vec3d& v10, const Vec3d& v01, const Vec3d& v11,
   	 std::vector<Vec3d>& points);

    int get_gridid(const int& x, const int& y, const int& z);
    void CollectQuadTuples(std::vector<Vec3dQuadTuple>& quadtuples);
    void SolveQuad(const Vec3dQuad& x, const Vec3dQuad& u, const Vec3dQuad& v, std::vector<Vec3d>& points);
    void SolveQuad(const Vec3dQuadTuple& data, std::vector<Vec3d>& points); 
    bool inside_range(const Eigen::Vector3d& p);
    bool inside_range(const Vec3dQuad& quad);

public:
    CosmicwebEngine() {};
    ~CosmicwebEngine() {};
    CosmicwebEngine(const CosmicwebOptions& options, const Eigen::MatrixXd& grid_points, const Eigen::VectorXd& vals, const std::vector<Eigen::Vector3d>& grads, const std::vector<Eigen::Vector3d>& hesxgrads)
        : options_(options), grid_points_(grid_points), vals_(vals), grads_(grads), hesxgrads_(hesxgrads) {
            resolution = std::round(pow(grid_points_.rows(), 1.0 / 3.0));
        };

    void Run(std::vector<Eigen::Vector3d>& points);
};

} // namespace vis21

#endif // VIS21_SRC_CORE_COSMICWEB_ENGINE_H

