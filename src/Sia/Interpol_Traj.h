#ifndef _Interpol_Traj_H
#define _Interpol_Traj_H

#include <Math/Vectorf.h>
#include <Math/Matrix4x4f.h>

class Interpol_Traj {
protected:
	Math::Vector3f new_pos;
	Math::Vector3f p0;
	Math::Vector3f d0;
	Math::Vector3f p1;
	Math::Vector3f d1;
public:
	Interpol_Traj(Math::Vector3f p0, Math::Vector3f d0, Math::Vector3f p1, Math::Vector3f d1) :p0(p0),d0(d0),p1(p1),d1(d1)
	{
	}
	Math::Vector3f SplineHermite(double u) {
		u = u - floor(u);
		float poly_1 = 2*pow(u,3) + -3 * pow(u, 2) + 1;
		float poly_2 = -2 * pow(u, 3) + 3 * pow(u, 2);
		float poly_3 = 1 * pow(u, 3) + -2 * pow(u, 2) + u;
		float poly_4 = 1 * pow(u, 3) + -1 * pow(u, 2);
		return p0*poly_1 + p1*poly_2 + d0*poly_3 + d1*poly_4;
	}
	Math::Vector3f vitesse(double u) {
		u = u - floor(u);
		float b0 = 6 * pow(u, 2) - 6 * u;
		float b1 = -6 * pow(u, 2) + 6 * u;
		float b2 = 3 * pow(u, 2) - 4 * u + 1;
		float b3 = 3 * pow(u, 2) - 2 * u;
		return p0*b0+p1*b1+d0*b2+d1*b3;
	}
};
#endif