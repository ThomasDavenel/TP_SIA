#include <Math/Vectorf.h>
#include <Math/Matrix4x4f.h>
#include <Sia/Interpol_Traj.h>
#include <vector>

class Interpol_Traj_v2 {
protected:
	Interpol_Traj * inter;
	std::vector<Math::Vector3f> & lposition;
	std::vector<Math::Vector3f> & lvitesse;
	int taille;
public:
	Interpol_Traj_v2(int taille, std::vector<Math::Vector3f> & lp, std::vector<Math::Vector3f> & lv):taille(taille),inter(nullptr),lposition(lp),lvitesse(lv)
	{
	}
	Math::Vector3f SplineHermite(double u) {
		int indice =  int(u) % (taille-1);
		//std::cout << indice << std::endl;
		delete(inter);
		inter = new Interpol_Traj(lposition[indice],lvitesse[indice],lposition[indice+1],lvitesse[indice+1]);
		u = u - floor(u);
		return inter->SplineHermite(u);
	}
	Math::Vector3f vitesse(double u) {
		u = u - floor(u);
		return inter->vitesse(u);
	}
};