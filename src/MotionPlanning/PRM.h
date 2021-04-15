#pragma once
#pragma once
#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/SixDofConfigurationGraph.h>
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
	class PRM : public SixDofPlannerBase
	{
		SixDofConfigurationGraph graphe;
		size_t k_voisin = 10;
	public:
		PRM(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::initializer_list<std::pair<float, float>>& intervals = { defaultPositionInterval(), defaultPositionInterval(), defaultPositionInterval(), defaultAngleInterval(), defaultAngleInterval(), defaultAngleInterval() }) :
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}

		PRM(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::vector<std::pair<float, float>>& intervals) :
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}
		void 	grow(size_t nbNodes, size_t k, float dq, size_t maxSamples = std::numeric_limits< size_t >::max()) {
			int current_nbNodes = 0;
			int current_sample = 0;
			while (current_nbNodes < nbNodes && current_sample < maxSamples) {
				//On cree une nouvelle config
				Configuration rnd_Config = randomConfiguration();
				//Si elle est sur le free space on l'ajoute au graphe
				if (!doCollide(rnd_Config)) graphe.add(rnd_Config);
				//On la relie a ses k voisin si c'est possible 
				for (auto c : graphe.kNearestNeighbours(rnd_Config, k)) {
					if (!doCollide(rnd_Config, c->getConfiguration(), dq)) {

					}
				}
			}
		}
		virtual bool 	plan(const Configuration &start, const Configuration &target, float radius, float dq, std::vector< Configuration > &result) override {

		}
	private:
	};
}