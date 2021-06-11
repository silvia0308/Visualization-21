#include "core/cosmic_web.h"
#include <iostream>

namespace vis21 {

bool CosmicwebEngine::PntContained(const Vec3d& candidate, const std::vector<Vec3d>& points)
{
 	// go through the list of point indices to check
 	for (const Vec3d& pc : points)
 	{
 	    // if an existing point is closer than the duplicate distance, return yes
 	    if ((pc - candidate).squaredNorm() < options_.eps_duplicate * options_.eps_duplicate)
 	   	 return true;
 	}
 	// no other point is in duplicate distance. seems to be a new point
 	return false;
}

void CosmicwebEngine::SolveQuad_Bezier(const Vec3d& x00, const Vec3d& x10, const Vec3d& x01, const Vec3d& x11,
   	 const Vec3d& u00, const Vec3d& u10, const Vec3d& u01, const Vec3d& u11,
   	 const Vec3d& v00, const Vec3d& v10, const Vec3d& v01, const Vec3d& v11,
   	 std::vector<Vec3d>& points)
{
   	 // do we have an empty space voxel? there is certainly no point in here.
   	 if (u00.squaredNorm() < options_.eps_null && u10.squaredNorm() < options_.eps_null && u01.squaredNorm() < options_.eps_null && u11.squaredNorm() < options_.eps_null
   		 || v00.squaredNorm() < options_.eps_null && v10.squaredNorm() < options_.eps_null && v01.squaredNorm() < options_.eps_null && v11.squaredNorm() < options_.eps_null)
   		 return;

   	 // quad small enough? terminate
   	 if ((x00 - x11).squaredNorm() < options_.eps_refine * options_.eps_refine)
   	 {
   		 // compute the center of the quad
   		 Vec3d xc = (x00 + x11) / 2;

   		 // iterate the previous vertices on this wall to check if there is one that is very close to the new one (potentially a duplicate)
   		 if (!PntContained(xc, points)) {
   		     points.push_back(xc);
   		 }
   		 return;
   	 }

   	 // compute bezier coefficients for the x-component of the cross product
   	 double a_bernstein[] = {
   		 u00.y()*v00.z() - u00.z()*v00.y(),
   		 (u00.y()*v01.z() - u00.z()*v01.y() + u01.y()*v00.z() - 2 * u00.y()*v00.z() - u01.z()*v00.y() + 2 * u00.z()*v00.y()) / 2 + u00.y()*v00.z() - u00.z()*v00.y(),
   		 u01.y()*v01.z() - u01.z()*v01.y(),
   		 (u00.y()*v10.z() - u00.z()*v10.y() + u10.y()*v00.z() - 2 * u00.y()*v00.z() - u10.z()*v00.y() + 2 * u00.z()*v00.y()) / 2 + u00.y()*v00.z() - u00.z()*v00.y(),
   		 (u00.y()*v11.z() - u00.z()*v11.y() + u01.y()*v10.z() - 2 * u00.y()*v10.z() - u01.z()*v10.y() + 2 * u00.z()*v10.y() + u10.y()*v01.z() - 2 * u00.y()*v01.z() - u10.z()*v01.y() + 2 * u00.z()*v01.y() + u11.y()*v00.z() - 2 * u10.y()*v00.z() - 2 * u01.y()*v00.z() + 4 * u00.y()*v00.z() - u11.z()*v00.y() + 2 * u10.z()*v00.y() + 2 * u01.z()*v00.y() - 4 * u00.z()*v00.y()) / 4 + (u00.y()*v10.z() - u00.z()*v10.y() + u10.y()*v00.z() - 2 * u00.y()*v00.z() - u10.z()*v00.y() + 2 * u00.z()*v00.y()) / 2 + (u00.y()*v01.z() - u00.z()*v01.y() + u01.y()*v00.z() - 2 * u00.y()*v00.z() - u01.z()*v00.y() + 2 * u00.z()*v00.y()) / 2 + u00.y()*v00.z() - u00.z()*v00.y(),
   		 (u01.y()*v11.z() - u00.y()*v11.z() - u01.z()*v11.y() + u00.z()*v11.y() - u01.y()*v10.z() + u00.y()*v10.z() + u01.z()*v10.y() - u00.z()*v10.y() + u11.y()*v01.z() - u10.y()*v01.z() - 2 * u01.y()*v01.z() + 2 * u00.y()*v01.z() - u11.z()*v01.y() + u10.z()*v01.y() + 2 * u01.z()*v01.y() - 2 * u00.z()*v01.y() - u11.y()*v00.z() + u10.y()*v00.z() + 2 * u01.y()*v00.z() - 2 * u00.y()*v00.z() + u11.z()*v00.y() - u10.z()*v00.y() - 2 * u01.z()*v00.y() + 2 * u00.z()*v00.y()) / 2 + (u00.y()*v11.z() - u00.z()*v11.y() + u01.y()*v10.z() - 2 * u00.y()*v10.z() - u01.z()*v10.y() + 2 * u00.z()*v10.y() + u10.y()*v01.z() - 2 * u00.y()*v01.z() - u10.z()*v01.y() + 2 * u00.z()*v01.y() + u11.y()*v00.z() - 2 * u10.y()*v00.z() - 2 * u01.y()*v00.z() + 4 * u00.y()*v00.z() - u11.z()*v00.y() + 2 * u10.z()*v00.y() + 2 * u01.z()*v00.y() - 4 * u00.z()*v00.y()) / 2 + (u00.y()*v10.z() - u00.z()*v10.y() + u10.y()*v00.z() - 2 * u00.y()*v00.z() - u10.z()*v00.y() + 2 * u00.z()*v00.y()) / 2 + u01.y()*v01.z() - u01.z()*v01.y(),
   		 u10.y()*v10.z() - u10.z()*v10.y(),
   		 (u10.y()*v11.z() - u00.y()*v11.z() - u10.z()*v11.y() + u00.z()*v11.y() + u11.y()*v10.z() - 2 * u10.y()*v10.z() - u01.y()*v10.z() + 2 * u00.y()*v10.z() - u11.z()*v10.y() + 2 * u10.z()*v10.y() + u01.z()*v10.y() - 2 * u00.z()*v10.y() - u10.y()*v01.z() + u00.y()*v01.z() + u10.z()*v01.y() - u00.z()*v01.y() - u11.y()*v00.z() + 2 * u10.y()*v00.z() + u01.y()*v00.z() - 2 * u00.y()*v00.z() + u11.z()*v00.y() - 2 * u10.z()*v00.y() - u01.z()*v00.y() + 2 * u00.z()*v00.y()) / 2 + (u00.y()*v11.z() - u00.z()*v11.y() + u01.y()*v10.z() - 2 * u00.y()*v10.z() - u01.z()*v10.y() + 2 * u00.z()*v10.y() + u10.y()*v01.z() - 2 * u00.y()*v01.z() - u10.z()*v01.y() + 2 * u00.z()*v01.y() + u11.y()*v00.z() - 2 * u10.y()*v00.z() - 2 * u01.y()*v00.z() + 4 * u00.y()*v00.z() - u11.z()*v00.y() + 2 * u10.z()*v00.y() + 2 * u01.z()*v00.y() - 4 * u00.z()*v00.y()) / 2 + u10.y()*v10.z() - u10.z()*v10.y() + (u00.y()*v01.z() - u00.z()*v01.y() + u01.y()*v00.z() - 2 * u00.y()*v00.z() - u01.z()*v00.y() + 2 * u00.z()*v00.y()) / 2,
   		 u11.y()*v11.z() - u11.z()*v11.y() };

   	 // use convex hull property to check, whether zero crossings exist for the scalar component
   	 if ((a_bernstein[0] > 0 && a_bernstein[1] > 0 && a_bernstein[2] > 0 && a_bernstein[3] > 0 && a_bernstein[4] > 0 && a_bernstein[5] > 0 && a_bernstein[6] > 0 && a_bernstein[7] > 0 && a_bernstein[8] > 0)
   		 || (a_bernstein[0] < 0 && a_bernstein[1] < 0 && a_bernstein[2] < 0 && a_bernstein[3] < 0 && a_bernstein[4] < 0 && a_bernstein[5] < 0 && a_bernstein[6] < 0 && a_bernstein[7] < 0 && a_bernstein[8] < 0))
   		 return;

   	 // compute bezier coefficients for the z-component of the cross product
   	 double b_bernstein[] = {
   		 u00.x()*v00.y() - u00.y()*v00.x(),
   		 (u00.x()*v01.y() - u00.y()*v01.x() + u01.x()*v00.y() - 2 * u00.x()*v00.y() - u01.y()*v00.x() + 2 * u00.y()*v00.x()) / 2 + u00.x()*v00.y() - u00.y()*v00.x(),
   		 u01.x()*v01.y() - u01.y()*v01.x(),
   		 (u00.x()*v10.y() - u00.y()*v10.x() + u10.x()*v00.y() - 2 * u00.x()*v00.y() - u10.y()*v00.x() + 2 * u00.y()*v00.x()) / 2 + u00.x()*v00.y() - u00.y()*v00.x(),
   		 (u00.x()*v11.y() - u00.y()*v11.x() + u01.x()*v10.y() - 2 * u00.x()*v10.y() - u01.y()*v10.x() + 2 * u00.y()*v10.x() + u10.x()*v01.y() - 2 * u00.x()*v01.y() - u10.y()*v01.x() + 2 * u00.y()*v01.x() + u11.x()*v00.y() - 2 * u10.x()*v00.y() - 2 * u01.x()*v00.y() + 4 * u00.x()*v00.y() - u11.y()*v00.x() + 2 * u10.y()*v00.x() + 2 * u01.y()*v00.x() - 4 * u00.y()*v00.x()) / 4 + (u00.x()*v10.y() - u00.y()*v10.x() + u10.x()*v00.y() - 2 * u00.x()*v00.y() - u10.y()*v00.x() + 2 * u00.y()*v00.x()) / 2 + (u00.x()*v01.y() - u00.y()*v01.x() + u01.x()*v00.y() - 2 * u00.x()*v00.y() - u01.y()*v00.x() + 2 * u00.y()*v00.x()) / 2 + u00.x()*v00.y() - u00.y()*v00.x(),
   		 (u01.x()*v11.y() - u00.x()*v11.y() - u01.y()*v11.x() + u00.y()*v11.x() - u01.x()*v10.y() + u00.x()*v10.y() + u01.y()*v10.x() - u00.y()*v10.x() + u11.x()*v01.y() - u10.x()*v01.y() - 2 * u01.x()*v01.y() + 2 * u00.x()*v01.y() - u11.y()*v01.x() + u10.y()*v01.x() + 2 * u01.y()*v01.x() - 2 * u00.y()*v01.x() - u11.x()*v00.y() + u10.x()*v00.y() + 2 * u01.x()*v00.y() - 2 * u00.x()*v00.y() + u11.y()*v00.x() - u10.y()*v00.x() - 2 * u01.y()*v00.x() + 2 * u00.y()*v00.x()) / 2 + (u00.x()*v11.y() - u00.y()*v11.x() + u01.x()*v10.y() - 2 * u00.x()*v10.y() - u01.y()*v10.x() + 2 * u00.y()*v10.x() + u10.x()*v01.y() - 2 * u00.x()*v01.y() - u10.y()*v01.x() + 2 * u00.y()*v01.x() + u11.x()*v00.y() - 2 * u10.x()*v00.y() - 2 * u01.x()*v00.y() + 4 * u00.x()*v00.y() - u11.y()*v00.x() + 2 * u10.y()*v00.x() + 2 * u01.y()*v00.x() - 4 * u00.y()*v00.x()) / 2 + (u00.x()*v10.y() - u00.y()*v10.x() + u10.x()*v00.y() - 2 * u00.x()*v00.y() - u10.y()*v00.x() + 2 * u00.y()*v00.x()) / 2 + u01.x()*v01.y() - u01.y()*v01.x(),
   		 u10.x()*v10.y() - u10.y()*v10.x(),
   		 (u10.x()*v11.y() - u00.x()*v11.y() - u10.y()*v11.x() + u00.y()*v11.x() + u11.x()*v10.y() - 2 * u10.x()*v10.y() - u01.x()*v10.y() + 2 * u00.x()*v10.y() - u11.y()*v10.x() + 2 * u10.y()*v10.x() + u01.y()*v10.x() - 2 * u00.y()*v10.x() - u10.x()*v01.y() + u00.x()*v01.y() + u10.y()*v01.x() - u00.y()*v01.x() - u11.x()*v00.y() + 2 * u10.x()*v00.y() + u01.x()*v00.y() - 2 * u00.x()*v00.y() + u11.y()*v00.x() - 2 * u10.y()*v00.x() - u01.y()*v00.x() + 2 * u00.y()*v00.x()) / 2 + (u00.x()*v11.y() - u00.y()*v11.x() + u01.x()*v10.y() - 2 * u00.x()*v10.y() - u01.y()*v10.x() + 2 * u00.y()*v10.x() + u10.x()*v01.y() - 2 * u00.x()*v01.y() - u10.y()*v01.x() + 2 * u00.y()*v01.x() + u11.x()*v00.y() - 2 * u10.x()*v00.y() - 2 * u01.x()*v00.y() + 4 * u00.x()*v00.y() - u11.y()*v00.x() + 2 * u10.y()*v00.x() + 2 * u01.y()*v00.x() - 4 * u00.y()*v00.x()) / 2 + u10.x()*v10.y() - u10.y()*v10.x() + (u00.x()*v01.y() - u00.y()*v01.x() + u01.x()*v00.y() - 2 * u00.x()*v00.y() - u01.y()*v00.x() + 2 * u00.y()*v00.x()) / 2,
   		 u11.x()*v11.y() - u11.y()*v11.x() };

   	 // use convex hull property to check, whether zero crossings exist for the scalar component
   	 if ((b_bernstein[0] > 0 && b_bernstein[1] > 0 && b_bernstein[2] > 0 && b_bernstein[3] > 0 && b_bernstein[4] > 0 && b_bernstein[5] > 0 && b_bernstein[6] > 0 && b_bernstein[7] > 0 && b_bernstein[8] > 0)
   		 || (b_bernstein[0] < 0 && b_bernstein[1] < 0 && b_bernstein[2] < 0 && b_bernstein[3] < 0 && b_bernstein[4] < 0 && b_bernstein[5] < 0 && b_bernstein[6] < 0 && b_bernstein[7] < 0 && b_bernstein[8] < 0))
   		 return;

   	 // compute bezier coefficients for the y-component of the cross product
   	 double c_bernstein[] = {
   		 u00.z()*v00.x() - u00.x()*v00.z(),
   		 (-u00.x()*v01.z() + u00.z()*v01.x() - u01.x()*v00.z() + 2 * u00.x()*v00.z() + u01.z()*v00.x() - 2 * u00.z()*v00.x()) / 2 - u00.x()*v00.z() + u00.z()*v00.x(),
   		 u01.z()*v01.x() - u01.x()*v01.z(),
   		 (-u00.x()*v10.z() + u00.z()*v10.x() - u10.x()*v00.z() + 2 * u00.x()*v00.z() + u10.z()*v00.x() - 2 * u00.z()*v00.x()) / 2 - u00.x()*v00.z() + u00.z()*v00.x(),
   		 (-u00.x()*v11.z() + u00.z()*v11.x() - u01.x()*v10.z() + 2 * u00.x()*v10.z() + u01.z()*v10.x() - 2 * u00.z()*v10.x() - u10.x()*v01.z() + 2 * u00.x()*v01.z() + u10.z()*v01.x() - 2 * u00.z()*v01.x() - u11.x()*v00.z() + 2 * u10.x()*v00.z() + 2 * u01.x()*v00.z() - 4 * u00.x()*v00.z() + u11.z()*v00.x() - 2 * u10.z()*v00.x() - 2 * u01.z()*v00.x() + 4 * u00.z()*v00.x()) / 4 + (-u00.x()*v10.z() + u00.z()*v10.x() - u10.x()*v00.z() + 2 * u00.x()*v00.z() + u10.z()*v00.x() - 2 * u00.z()*v00.x()) / 2 + (-u00.x()*v01.z() + u00.z()*v01.x() - u01.x()*v00.z() + 2 * u00.x()*v00.z() + u01.z()*v00.x() - 2 * u00.z()*v00.x()) / 2 - u00.x()*v00.z() + u00.z()*v00.x(),
   		 (-u01.x()*v11.z() + u00.x()*v11.z() + u01.z()*v11.x() - u00.z()*v11.x() + u01.x()*v10.z() - u00.x()*v10.z() - u01.z()*v10.x() + u00.z()*v10.x() - u11.x()*v01.z() + u10.x()*v01.z() + 2 * u01.x()*v01.z() - 2 * u00.x()*v01.z() + u11.z()*v01.x() - u10.z()*v01.x() - 2 * u01.z()*v01.x() + 2 * u00.z()*v01.x() + u11.x()*v00.z() - u10.x()*v00.z() - 2 * u01.x()*v00.z() + 2 * u00.x()*v00.z() - u11.z()*v00.x() + u10.z()*v00.x() + 2 * u01.z()*v00.x() - 2 * u00.z()*v00.x()) / 2 + (-u00.x()*v11.z() + u00.z()*v11.x() - u01.x()*v10.z() + 2 * u00.x()*v10.z() + u01.z()*v10.x() - 2 * u00.z()*v10.x() - u10.x()*v01.z() + 2 * u00.x()*v01.z() + u10.z()*v01.x() - 2 * u00.z()*v01.x() - u11.x()*v00.z() + 2 * u10.x()*v00.z() + 2 * u01.x()*v00.z() - 4 * u00.x()*v00.z() + u11.z()*v00.x() - 2 * u10.z()*v00.x() - 2 * u01.z()*v00.x() + 4 * u00.z()*v00.x()) / 2 + (-u00.x()*v10.z() + u00.z()*v10.x() - u10.x()*v00.z() + 2 * u00.x()*v00.z() + u10.z()*v00.x() - 2 * u00.z()*v00.x()) / 2 - u01.x()*v01.z() + u01.z()*v01.x(),
   		 u10.z()*v10.x() - u10.x()*v10.z(),
   		 (-u10.x()*v11.z() + u00.x()*v11.z() + u10.z()*v11.x() - u00.z()*v11.x() - u11.x()*v10.z() + 2 * u10.x()*v10.z() + u01.x()*v10.z() - 2 * u00.x()*v10.z() + u11.z()*v10.x() - 2 * u10.z()*v10.x() - u01.z()*v10.x() + 2 * u00.z()*v10.x() + u10.x()*v01.z() - u00.x()*v01.z() - u10.z()*v01.x() + u00.z()*v01.x() + u11.x()*v00.z() - 2 * u10.x()*v00.z() - u01.x()*v00.z() + 2 * u00.x()*v00.z() - u11.z()*v00.x() + 2 * u10.z()*v00.x() + u01.z()*v00.x() - 2 * u00.z()*v00.x()) / 2 + (-u00.x()*v11.z() + u00.z()*v11.x() - u01.x()*v10.z() + 2 * u00.x()*v10.z() + u01.z()*v10.x() - 2 * u00.z()*v10.x() - u10.x()*v01.z() + 2 * u00.x()*v01.z() + u10.z()*v01.x() - 2 * u00.z()*v01.x() - u11.x()*v00.z() + 2 * u10.x()*v00.z() + 2 * u01.x()*v00.z() - 4 * u00.x()*v00.z() + u11.z()*v00.x() - 2 * u10.z()*v00.x() - 2 * u01.z()*v00.x() + 4 * u00.z()*v00.x()) / 2 - u10.x()*v10.z() + u10.z()*v10.x() + (-u00.x()*v01.z() + u00.z()*v01.x() - u01.x()*v00.z() + 2 * u00.x()*v00.z() + u01.z()*v00.x() - 2 * u00.z()*v00.x()) / 2,
   		 u11.z()*v11.x() - u11.x()*v11.z() };

   	 // use convex hull property to check, whether zero crossings exist for the scalar component
   	 if ((c_bernstein[0] > 0 && c_bernstein[1] > 0 && c_bernstein[2] > 0 && c_bernstein[3] > 0 && c_bernstein[4] > 0 && c_bernstein[5] > 0 && b_bernstein[6] > 0 && c_bernstein[7] > 0 && c_bernstein[8] > 0)
   		 || (c_bernstein[0] < 0 && c_bernstein[1] < 0 && c_bernstein[2] < 0 && c_bernstein[3] < 0 && c_bernstein[4] < 0 && c_bernstein[5] < 0 && b_bernstein[6] < 0 && c_bernstein[7] < 0 && c_bernstein[8] < 0))
   		 return;

   	 // there is a crossing inside the quad -> subdivide

   	 //  01 - d1 - 11
   	 //   |	|	|
   	 //  0d - dd - 1d
   	 //   |	|	|
   	 //  00 - d0 - 10

   	 Vec3d x0d = (x00 + x01)*0.5; Vec3d x1d = (x10 + x11)*0.5; Vec3d xd0 = (x00 + x10)*0.5; Vec3d xd1 = (x01 + x11)*0.5; Vec3d xdd = (xd0 + xd1)*0.5;
   	 Vec3d u0d = (u00 + u01)*0.5; Vec3d u1d = (u10 + u11)*0.5; Vec3d ud0 = (u00 + u10)*0.5; Vec3d ud1 = (u01 + u11)*0.5; Vec3d udd = (ud0 + ud1)*0.5;
   	 Vec3d v0d = (v00 + v01)*0.5; Vec3d v1d = (v10 + v11)*0.5; Vec3d vd0 = (v00 + v10)*0.5; Vec3d vd1 = (v01 + v11)*0.5; Vec3d vdd = (vd0 + vd1)*0.5;

   	 SolveQuad_Bezier(x00, xd0, x0d, xdd, u00, ud0, u0d, udd, v00, vd0, v0d, vdd, points);
   	 SolveQuad_Bezier(xd0, x10, xdd, x1d, ud0, u10, udd, u1d, vd0, v10, vdd, v1d, points);
   	 SolveQuad_Bezier(x0d, xdd, x01, xd1, u0d, udd, u01, ud1, v0d, vdd, v01, vd1, points);
   	 SolveQuad_Bezier(xdd, x1d, xd1, x11, udd, u1d, ud1, u11, vdd, v1d, vd1, v11, points);
}

int CosmicwebEngine::get_gridid(const int& x, const int& y, const int& z) {
    return (x * resolution + y) * resolution + z;
}

bool CosmicwebEngine::inside_range(const Eigen::Vector3d& p) {
    const double range_min = 0.0;
    const double range_max = 64.0;
    if (p.x() < range_min || p.x() > range_max)
        return false;
    if (p.y() < range_min || p.y() > range_max)
        return false;
    if (p.z() < range_min || p.z() > range_max)
        return false;
    return true;
}

bool CosmicwebEngine::inside_range(const Vec3dQuad& quad) {
    return inside_range(quad.p00) && inside_range(quad.p01) && inside_range(quad.p10) && inside_range(quad.p11);
}

void CosmicwebEngine::CollectQuadTuples(std::vector<Vec3dQuadTuple>& quadtuples) {
    // yz
    for (int x = 0; x < resolution; ++x) {
        for (int y = 0; y < resolution - 1; ++y) {
            for (int z = 0; z < resolution - 1; ++z) {
                int id_00 = get_gridid(x, y, z);
                int id_10 = get_gridid(x, y+1, z);
                int id_01 = get_gridid(x, y, z+1);
                int id_11 = get_gridid(x, y+1, z+1);
                Vec3dQuad quadx(grid_points_.row(id_00), grid_points_.row(id_10), grid_points_.row(id_01), grid_points_.row(id_11));
                if (!inside_range(quadx))
                    continue;
                Vec3dQuad quadu(grads_[id_00], grads_[id_10], grads_[id_01], grads_[id_11]);
                Vec3dQuad quadv(hesxgrads_[id_00], hesxgrads_[id_10], hesxgrads_[id_01], hesxgrads_[id_11]);
                Vec3dQuadTuple quadtuple(quadx, quadu, quadv);
                quadtuples.push_back(quadtuple);
            }
        }
    }
    // xz
    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution - 1; ++x) {
            for (int z = 0; z < resolution - 1; ++z) {
                int id_00 = get_gridid(x, y, z);
                int id_10 = get_gridid(x+1, y, z);
                int id_01 = get_gridid(x, y, z+1);
                int id_11 = get_gridid(x+1, y, z+1);
                Vec3dQuad quadx(grid_points_.row(id_00), grid_points_.row(id_10), grid_points_.row(id_01), grid_points_.row(id_11));
                if (!inside_range(quadx))
                    continue;
                Vec3dQuad quadu(grads_[id_00], grads_[id_10], grads_[id_01], grads_[id_11]);
                Vec3dQuad quadv(hesxgrads_[id_00], hesxgrads_[id_10], hesxgrads_[id_01], hesxgrads_[id_11]);
                Vec3dQuadTuple quadtuple(quadx, quadu, quadv);
                quadtuples.push_back(quadtuple);
            }
        }
    }
    // xy
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution - 1; ++x) {
            for (int y = 0; y < resolution - 1; ++y) {
                int id_00 = get_gridid(x, y, z);
                int id_10 = get_gridid(x+1, y, z);
                int id_01 = get_gridid(x, y+1, z);
                int id_11 = get_gridid(x+1, y+1, z);
                Vec3dQuad quadx(grid_points_.row(id_00), grid_points_.row(id_10), grid_points_.row(id_01), grid_points_.row(id_11));
                if (!inside_range(quadx))
                    continue;
                Vec3dQuad quadu(grads_[id_00], grads_[id_10], grads_[id_01], grads_[id_11]);
                Vec3dQuad quadv(hesxgrads_[id_00], hesxgrads_[id_10], hesxgrads_[id_01], hesxgrads_[id_11]);
                Vec3dQuadTuple quadtuple(quadx, quadu, quadv);
                quadtuples.push_back(quadtuple);
            }
        }
    }
}

void CosmicwebEngine::SolveQuad(const Vec3dQuad& x, const Vec3dQuad& u, const Vec3dQuad& v, std::vector<Vec3d>& points) {
    SolveQuad_Bezier(x.p00, x.p10, x.p01, x.p11,
                     u.p00, u.p10, u.p01, u.p11,
                     v.p00, v.p10, v.p01, v.p11,
                     points);
}

void CosmicwebEngine::SolveQuad(const Vec3dQuadTuple& data, std::vector<Vec3d>& points) {
    SolveQuad(data.x, data.u, data.v, points);
}

void CosmicwebEngine::Run(std::vector<Vec3d>& points) {
    // collect all quadtuples
    std::vector<Vec3dQuadTuple> quadtuples;
    CollectQuadTuples(quadtuples);

    // solve quad
    std::vector<std::vector<Vec3d>> points_array;
    points_array.resize(quadtuples.size());
// #pragma omp parallel for
    for (int i = 0; i < quadtuples.size(); ++i) {
        // if ((i+1) % 100000 == 0)
        //     std::cout<<"solve quadtuple ("<<i+1<<" / "<<quadtuples.size()<<")"<<std::endl;
        SolveQuad(quadtuples[i], points_array[i]);
    }

    // collect points
    std::vector<double> sizes_vec;
    sizes_vec.resize(quadtuples.size());
    int counter = 0;
    for (int i = 0; i < quadtuples.size(); ++i) {
        sizes_vec[i] = points_array[i].size();
        if (sizes_vec[i] > 0)
            counter++;
    }
    std::cout<<"number of valid faces = "<<counter<<", quadtuples.size() = "<<quadtuples.size()<<std::endl;
    for (int i = 0; i < quadtuples.size(); ++i) {
        points.insert(points.end(), points_array[i].begin(), points_array[i].end());
    }
}

} // namespace vis21

