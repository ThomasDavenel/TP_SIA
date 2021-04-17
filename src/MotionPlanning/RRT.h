#pragma once
#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/SixDofConfigurationTree.h>
#include <Math/Vectorf.h>
#include <Math/UniformRandom.h>
#include <Math/Interpolation.h>
#include <Math/Quaternion.h>
#include <MotionPlanning/CollisionManager.h>
#include <Math/Constant.h>
#include <vector>

namespace MotionPlanning
{
	/// <summary>
	/// Base class for six dof planners
	/// </summary>
	class RRT : public SixDofPlannerBase
	{
		MotionPlanning::SixDofConfigurationTree arbre;

	public:
		RRT::RRT(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::initializer_list<std::pair<float, float>>& intervals = { defaultPositionInterval(), defaultPositionInterval(), defaultPositionInterval(), defaultAngleInterval(), defaultAngleInterval(), defaultAngleInterval() }):
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}

		RRT::RRT(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::vector<std::pair<float, float>>& intervals) :
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}

		bool plan(const Configuration & start, const Configuration & target, float radius, float dq, std::vector<Configuration> & result) {
			arbre.createNode(start);
			bool linked = false;
			int cpt = 0;
			int maxIter = 1000000;
			while (!linked && cpt<maxIter) {
				//On créer Qrand
				Configuration rnd_Config = randomConfiguration();
				MotionPlanning::SixDofConfigurationTree::Node * nearest = arbre.nearest(rnd_Config);
				if (nearest == nullptr) {
					std::cout << "NULLPTR";
				}
				//on créer Qnew
				Configuration qNew;
				float distance = configurationDistance(rnd_Config,nearest->getConfiguration());
				if (distance > radius) {
					qNew = nearest->getConfiguration().interpolate(rnd_Config,radius/distance);
				}
				else {
					qNew = rnd_Config;
				}
				//Si le point est sur le free space et qu'on peut relié alors on l'ajoute a notre arbre
				if (!doCollide(qNew)) {
					if (!doCollide(qNew, nearest->getConfiguration(), dq)) {
						MotionPlanning::SixDofConfigurationTree::Node * node_qNew =arbre.createNode(qNew, nearest);
						//std::cout << "AJOUT" << std::endl;
						//On regarde si l'on peut relier notre nouvelle config et notre target
						if (true || configurationDistance(qNew, target)<radius) {
							//std::cout << "maybe" << std::endl;
							if (!doCollide(qNew, target, dq)) {
								std::cout << "Trouve" << std::endl;
								MotionPlanning::SixDofConfigurationTree::Node * node_final = arbre.createNode(target, node_qNew);
								creationVecteurResult(node_final,result);
								linked = true;
							}
						}
					}
				}
				cpt++;
			}
			if (!linked) {
				creationVecteurResult(arbre.nearest(target), result);
			}
			std::reverse(result.begin(), result.end());
			optimise(result, dq);
			int nbIter = 100;
			optimiseBis(result, dq,nbIter);
			return linked;
		}
	private:
		void optimise(std::vector<Configuration> & result, float dq) {
			bool changement = false;
			auto i = result.begin()+1;
			while (i != result.end()-1) {
				if (!doCollide(*(i - 1), *(i + 1), dq)) {
					i=result.erase(i);
					changement = true;
				}
				else {
					i++;
				}
			}
			if (changement) optimise(result, dq);
		}
		void optimiseBis(std::vector<Configuration> & result, float dq,int nbIter) {
			for (int i = 0; i < nbIter;i++) {		
				if (result.size() > 2) {
					int rnd1 =(int)((rand() % (result.size() - 2))) + 1;
					//std::cout << rnd1 << std::endl;
					float rnd2 = (float)((rand() % 100 + 1)) / 100;
					Configuration newC = result[rnd1 - 1].interpolate(result[rnd1], rnd2);
					if (!doCollide(newC, result[rnd1 + 1], dq)) {
						*(result.begin()+rnd1) = newC;
						std::cout << "opti2" << std::endl;
					}
				}
			}
		}
		void creationVecteurResult(MotionPlanning::SixDofConfigurationTree::Node * node_finale, std::vector<Configuration> & result) {
			result.push_back(node_finale->getConfiguration());
			if (node_finale->getFather() != nullptr) creationVecteurResult(node_finale->getFather(), result);
		}
	};
}