#ifndef VIS21_SRC_UTILS_INTERP_UTILS_H_
#define VIS21_SRC_UTILS_INTERP_UTILS_H_

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string.h>
#include <Eigen/Core>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

namespace vis21 {

// Kernel Functions: cubic spline
static double Bspline(double x)					// Cubic Bspline
{
	double W;
	x = fabs(x);

	if (x < 0.5)
		W = 6.0 * x * x * (x - 1) + 1;

	else if (x < 1.0)
		W = 2.0 * (1 - x) * (1 - x) * (1 - x);

	else
		W = 0;

	return W;
}

static double dBspline(double x)					// Cubic Bspline derivative
{
	double dW;
	double x_abs;
	x_abs = fabs(x);
    double sign_x = x / x_abs;
    x = x_abs;

	if (x < 0.5)
		dW = 18.0 * x * x * sign_x - 12.0 * x;

	else if (x < 1.0)
		dW = - 6 * (1 - x) * (1 - x) * sign_x;

	else
		dW = 0;

	return dW;
}

static double d2Bspline(double x) 
{
    double d2W;
    double x_abs;
    x_abs = fabs(x);
    double sign_x = x / x_abs;
    x = x_abs;

    if (x < 0.5)
        d2W = 36 * x - 12;

    else if (x < 1.0)
        d2W = 12 * (1 - x);
    
    else 
        d2W = 0;

    return d2W;
}

static float getw_ip(const Eigen::Vector3d& dist)		// 3D weight (independent)
{
    Eigen::Vector3d distx2 = dist * 2;
	return Bspline(distx2[0]) * Bspline(distx2[1]) * Bspline(distx2[2]);
}

static Eigen::Vector3d getdw_ip(const Eigen::Vector3d& dist)	// 3D weight gradient (independent)
{
    Eigen::Vector3d distx2 = dist * 2;
	return Eigen::Vector3d(
		dBspline(distx2[0]) * Bspline(distx2[1]) * Bspline(distx2[2]),
		Bspline(distx2[0]) * dBspline(distx2[1]) * Bspline(distx2[2]),
		Bspline(distx2[0]) * Bspline(distx2[1]) * dBspline(distx2[2]));
}

static Eigen::Matrix3d getd2w_ip(const Eigen::Vector3d& dist) // 3d weight hessian (independent)
{
    Eigen::Vector3d distx2 = dist * 2;
    Eigen::Matrix3d M;
    M(0, 0) = d2Bspline(distx2[0]) * Bspline(distx2[1]) * Bspline(distx2[2]);
    M(1, 1) = Bspline(distx2[0]) * d2Bspline(distx2[1]) * Bspline(distx2[2]);
    M(2, 2) = Bspline(distx2[0]) * Bspline(distx2[1]) * d2Bspline(distx2[2]);
    M(0, 1) = M(1, 0) = dBspline(distx2[0]) * dBspline(distx2[1]) * Bspline(distx2[2]);
    M(0, 2) = M(2, 0) = dBspline(distx2[0]) * Bspline(distx2[1]) * dBspline(distx2[2]);
    M(1, 2) = M(2, 1) = Bspline(distx2[0]) * dBspline(distx2[1]) * dBspline(distx2[2]);
    return M;
} 

Eigen::Matrix3d get_hessian(const Eigen::Vector3d& dist, const double& r, const double& dw, const double& d2w) {
    double val1 = dw / r;
    double val2 = d2w / (r * r) - dw / (r * r * r);
    Eigen::Matrix3d M;
    M(0, 0) = val1 + val2 * dist[0] * dist[0];
    M(1, 1) = val1 + val2 * dist[1] * dist[1];
    M(2, 2) = val1 + val2 * dist[2] * dist[2];
    M(0, 1) = M(1, 0) = val2 * dist[0] * dist[1];
    M(0, 2) = M(2, 0) = val2 * dist[0] * dist[2];
    M(1, 2) = M(2, 1) = val2 * dist[1] * dist[2];
    return M;
}

static float getw(const Eigen::Vector3d& dist) { // 3d weight
    return Bspline(dist.norm());
}

static Eigen::Vector3d getdw(const Eigen::Vector3d& dist) { // 3d weight gradient
    double r = dist.norm() + 1e-12;
    double dw = dBspline(dist.norm());
    return dw * dist / r;
}

// reference of the derivation: https://sunlimingbit.wordpress.com/2018/09/23/hessian-of-radial-functions/
static Eigen::Matrix3d getd2w(const Eigen::Vector3d& dist) { // 3d weight hessian
    double r = dist.norm() + 1e-12;
    double dw = dBspline(dist.norm());
    double d2w = d2Bspline(dist.norm());
    return get_hessian(dist, r, dw, d2w);
}

static double WC4(double x, double H) // reference: https://cgl.ethz.ch/Downloads/Publications/Papers/2019/Fri19a/Fri19a.pdf
{
    double W;
    if (x < 1) {
        double coeff = 495.0 / (32.0 * M_PI * pow(H, 3));
        W = coeff * pow(1 - x, 6) * (1 + 6 * x + 35.0 * x * x / 3.0);
    }
    else
        W = 0;
    return W;
}

static double dWC4(double x, double H)
{
    double dW;
    if (x < 1) {
        double coeff = 495.0 / (32.0 * M_PI * pow(H, 3));
        double t1 = pow(1 - x, 6);
        double dt1 = - 6 * pow(1 - x, 5);
        double t2 = 1 + 6 * x + 35.0 * x * x / 3.0;
        double dt2 = 6.0 + 70.0 * x / 3.0;
        dW = coeff * (dt1 * t2 + t1 * dt2);
    }
    else
        dW = 0;
    return dW;
}

static double d2WC4(double x, double H) 
{
    double d2W;
    if (x < 1) {
        double coeff = 495.0 / (32.0 * M_PI * pow(H, 3));
        double t1 = pow(1 - x, 6);
        double dt1 = - 6 * pow(1 - x, 5);
        double d2t1 = 30 * pow(1 - x, 4);
        double t2 = 1 + 6 * x + 35.0 * x * x / 3.0;
        double dt2 = 6.0 + 70.0 * x / 3.0;
        double d2t2 = 70.0 / 3.0;
        d2W = coeff * (d2t1 * t2 + 2 * dt1 * dt2 + t1 * d2t2);
    }
    else
        d2W = 0;
    return d2W;
}

static float getw_wc4(const Eigen::Vector3d& dist, double H) { // 3d weight
    return WC4(dist.norm(), H);
}

static Eigen::Vector3d getdw_wc4(const Eigen::Vector3d& dist, double H) { // 3d weight gradient
    double r = dist.norm() + 1e-12;
    double dw = dWC4(dist.norm(), H);
    return dw * dist / r;
}

// reference of the derivation: https://sunlimingbit.wordpress.com/2018/09/23/hessian-of-radial-functions/
static Eigen::Matrix3d getd2w_wc4(const Eigen::Vector3d& dist, double H) { // 3d weight hessian
    double r = dist.norm() + 1e-12;
    double dw = dWC4(dist.norm(), H);
    double d2w = d2WC4(dist.norm(), H);
    return get_hessian(dist, r, dw, d2w);
}

} // namespace vis21

#endif  // VOROCRUST_SRC_UTILS_INTERP_UTILS_H_

