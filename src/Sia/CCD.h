#include <Math/Vectorf.h>
#include <Math/finite.h>
#include <Animation/KinematicChain.h>
#include <SceneGraph/Group.h>

class CCD {
protected:
	Animation::KinematicChain * m_kinematicChain;
	Animation::KinematicChain::Node * m_noeud;
	float m_epsilon = 0.01f;
public:
	CCD(Animation::KinematicChain * m_k, Animation::KinematicChain::Node * m_n):m_kinematicChain(m_k), m_noeud(m_n)
	{
	}
	void convergetoward(Math::Vector3f decalage, float variationAngulaireMax) {
		::std::vector<Animation::KinematicChain::DegreeOfFreedom> collected;
		m_noeud->collectDegreesOfFreedom(collected);
		for (Animation::KinematicChain::DegreeOfFreedom dof : collected) {
			private_converge(decalage,dof, variationAngulaireMax);
		}
	}
	bool solve(Math::Vector3f decalage, float variationAngulaireMax) {
		convergetoward(decalage, variationAngulaireMax);
		float distance = sqrt(pow(decalage[0],2) + pow(decalage[1],2) + pow(decalage[1],2));
		//std::cout << distance << std::endl;
		return (distance < 0.15);
	}
private:
	void private_converge(Math::Vector3f decalage,Animation::KinematicChain::DegreeOfFreedom dof, float VariationAngulaireMax) {
		Math::Vector3f derivate = m_kinematicChain->derivate(m_noeud, decalage, dof, m_epsilon);
		float deltaAngle = ::Math::Interval<float>(-VariationAngulaireMax, VariationAngulaireMax).clamp(derivate.inv()*decalage);
		if (Math::is_valid(deltaAngle))
		{
			dof = dof + deltaAngle;
		}
	}
};