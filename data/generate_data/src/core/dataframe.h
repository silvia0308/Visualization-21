#ifndef VIS21_SRC_CORE_DATAFRAME_H
#define VIS21_SRC_CORE_DATAFRAME_H

#include <Eigen/Core>

namespace vis21 {

namespace ParticleType {
enum AttributeType {
    VX=0,
    VY=1,
    VZ=2,
    MASS=3,
    UU=4,
    HH=5,
    MU=6,
    RHO=7,
    PHI=8,
    TEMPERATURE=9,
    ENTROPY=10
};
}

class Particle {
public:
    Particle() {}
    ~Particle() {}
    Particle(const Eigen::Vector3d &p): p(p) {}
    
    Eigen::Vector3d p;
    Eigen::Vector3d v;
    double attr_data[8];
    long int id;
    unsigned short mask;

    double get_mass() {return get_attr(ParticleType::MASS);}
    double get_attr(const ParticleType::AttributeType &type) {
        int t = type;
        if (t <= 2) // velocity
            return v(t);
        else // velocity
            return attr_data[t - 3];
    }
    void set_attr(const ParticleType::AttributeType &type, const float &val) {
        int t = type;
        if (t <= 2) // velocity
            v(t) = val;
        else 
            attr_data[t - 3] = val;
    }
};

class DataFrame {
public:
    int frame_id;
    DataFrame() {}
    ~DataFrame() {}
    DataFrame(const std::string& filename, const int frame_id): frame_id(frame_id) {readFromFile(filename);}

    std::vector<Particle> particles;
    void readFromFile(const std::string& filename);
};

} // namespace vis21

#endif

