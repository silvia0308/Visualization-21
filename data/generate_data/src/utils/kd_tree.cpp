#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "utils/nanoflann.hpp"
#include "utils/kd_tree.h"

using namespace std;
using namespace nanoflann;

namespace vis21 {

bool KDTree::isInitialized() {
    int count = int(cloud.pts.size());
    return count > 0;
}

void KDTree::initialize(const Eigen::MatrixXd &points) {
    std::vector<Eigen::VectorXd> points_tmp;
    for (int i = 0; i < points.rows(); ++i) {
        points_tmp.push_back(points.row(i));
    }
    initialize(points_tmp);
}

void KDTree::initialize(const std::vector<Eigen::VectorXd> &points) {
    // if (options_.do_print) dump_mem_usage();

    cloud.pts = points;
    index = std::unique_ptr<my_kd_tree_t>(new my_kd_tree_t(3, cloud, KDTreeSingleIndexAdaptorParams(10)));
    index->buildIndex();

    // if (options_.do_print) dump_mem_usage();
    std::cout<<"KD tree initialized ("<<points.size()<<" points)"<<std::endl;
}

void KDTree::query_knn(Eigen::Vector3d query_pt, std::vector<int>& nearestVerticesIdx, size_t num_results) {
    // ----------------------------------------------------------------
    // knnSearch():  Perform a search for the N closest points
    // ----------------------------------------------------------------
    std::vector<size_t>   ret_index(num_results);
    std::vector<double> out_dist_sqr(num_results);

    double query_pt_tmp[3] = {query_pt(0), query_pt(1), query_pt(2)}; 
    num_results = index->knnSearch(&query_pt_tmp[0], num_results, &ret_index[0], &out_dist_sqr[0]);
    
    // In case of less points in the tree than requested:
    ret_index.resize(num_results);
    out_dist_sqr.resize(num_results);

    // construct nearest vertices
    nearestVerticesIdx.clear();
    for (size_t i = 0; i < num_results; i++)
        nearestVerticesIdx.push_back(ret_index[i]);

    // printing results
    if (options_.do_print) {
        cout << "knnSearch(): num_results=" << num_results << "\n";
        for (size_t i = 0; i < num_results; i++)
            cout << "idx["<< i << "]=" << ret_index[i] << " dist["<< i << "]=" << out_dist_sqr[i] << endl;
        cout << "\n";
    }
}

void KDTree::query_radius_search(Eigen::Vector3d query_pt, std::vector<int>& nearestVerticesIdx, double search_radius) {
    // ----------------------------------------------------------------
    // radiusSearch(): Perform a search for the points within search_radius
    // ----------------------------------------------------------------
    // const double search_radius = static_cast<double>(0.1);


    // radius search
    std::vector<std::pair<size_t,double> >   ret_matches;
    nanoflann::SearchParams params;
    double query_pt_tmp[3] = {query_pt[0], query_pt[1], query_pt[2]}; 
    const size_t nMatches = index->radiusSearch(&query_pt_tmp[0], search_radius * search_radius, ret_matches, params);

    // get nearest vertices index
    nearestVerticesIdx.clear();
    for (size_t i = 0; i < nMatches; i++)
        nearestVerticesIdx.push_back(ret_matches[i].first);

    // printing results
    if (options_.do_print) {
        cout << "radiusSearch(): radius=" << search_radius << " -> " << nMatches << " matches\n";
        for (size_t i = 0; i < nMatches; i++)
            cout << "idx["<< i << "]=" << ret_matches[i].first << " dist["<< i << "]=" << ret_matches[i].second << endl;
        cout << endl;
    }
}

} //namespace vis21

