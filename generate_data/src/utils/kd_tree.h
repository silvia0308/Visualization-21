#ifndef VIS21_SRC_UTILS_KD_TREE_H_
#define VIS21_SRC_UTILS_KD_TREE_H_

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string.h>

#include <Eigen/Core>
#include "utils/nanoflann.hpp"

using namespace nanoflann;
using namespace std;
namespace vis21 {

template <typename T>
struct PointCloud
{
    std::vector<Eigen::VectorXd> pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { 
        return pts.size();
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline T kdtree_get_pt(const size_t idx, const size_t dim) const
    {
        //if (dim == 0) return pts[idx].x;
        //else if (dim == 1) return pts[idx].y;
        //else return pts[idx].z;
        if (dim == 0) return pts[idx](0);
        else if (dim == 1) return pts[idx](1);
        else return pts[idx](2);
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }

};

struct KDTreeOptions {
    bool do_print = false; 
};

class KDTree {
public:
    // Originally an empty tree pointer here. 
    // Filled with function initialize(const Eigen::MatrixXd &points)
    static vector<int> DEFAULT_VECTOR;
    typedef KDTreeSingleIndexAdaptor<
       L2_Simple_Adaptor<double, PointCloud<double>> ,
       PointCloud<double>,
       3 /* dim */
       > my_kd_tree_t;
    std::unique_ptr<my_kd_tree_t> index = NULL;

    KDTreeOptions options_;
    PointCloud<double> cloud;

    KDTree() {}
    ~KDTree() {}
    KDTree(const KDTreeOptions &options): options_(options) {}

    bool isInitialized();
    std::vector<Eigen::VectorXd> all_points() { return cloud.pts; }
    Eigen::VectorXd point(const int &id) { if (id >= cloud.pts.size()) {throw std::runtime_error("query id is out of range");} else {return cloud.pts[id];} }

    void initialize(const std::vector<Eigen::VectorXd>& points);
    void initialize(const Eigen::MatrixXd& points);
    void query_knn(Eigen::Vector3d query_pt, std::vector<int>& nearestVerticesIdx, size_t num_results=5);
    void query_radius_search(Eigen::Vector3d query_pt, std::vector<int>& nearestVerticesIdx, double search_radius=0.62);
};

} // namespace vis21

inline void dump_mem_usage()
{
    FILE* f = fopen("/proc/self/statm","rt");
    if (!f) return;
    char str[300];
    size_t n = fread(str, 1, 200, f);
    str[n] = 0;
    printf("MEM: %s\n", str);
    fclose(f);
}

#endif  // VOROCRUST_SRC_UTILS_KD_TREE_H_

