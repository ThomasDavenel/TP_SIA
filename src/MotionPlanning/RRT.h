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
		MotionPlanning::SixDofConfigurationTree arbre();
	public:
		RRT::RRT(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::initializer_list<std::pair<float, float>>& intervals):
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}

		RRT::RRT(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::vector<std::pair<float, float>>& intervals) :
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}

		bool plan(const Configuration & start, const Configuration & target, float radius, float dq, std::vector<Configuration> & result) {
			bool linked = false;
			while (!linked) {

			}
		}
	};
}