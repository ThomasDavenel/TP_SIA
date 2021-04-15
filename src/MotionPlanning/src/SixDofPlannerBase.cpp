#include <MotionPlanning/SixDofPlannerBase.h>

namespace MotionPlanning
{
	SixDofPlannerBase::SixDofPlannerBase(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::initializer_list<std::pair<float, float>>& intervals):
		m_intervals(intervals), m_collisionManager(collisionManager), m_object(object)
	{
		assert(m_intervals.size() == 6);
	}

	SixDofPlannerBase::SixDofPlannerBase(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::vector<std::pair<float, float>>& intervals)
	: m_intervals(intervals), m_collisionManager(collisionManager), m_object(object)
	{
		assert(m_intervals.size() == 6);
	}

	SixDofPlannerBase::Configuration SixDofPlannerBase::randomConfiguration() const
	{
		float dofs[6];
		for (size_t cpt = 0; cpt<6; ++cpt)
		{
			dofs[cpt] = static_cast<float>(m_uniformRandom(m_intervals[cpt].first, m_intervals[cpt].second));
		}
		return Configuration{ Math::makeVector(dofs[0], dofs[1], dofs[2]), Math::makeVector(dofs[3], dofs[4], dofs[5]) };
	}

	float SixDofPlannerBase::distanceToObstacles(const Configuration & configuration) const
	{
		m_object.setTranslation(configuration.m_translation);
		m_object.setOrientation(configuration.m_orientation);
		return m_collisionManager->computeDistance();
	}

	bool SixDofPlannerBase::doCollide(const Configuration & configuration) const
	{
		m_object.setTranslation(configuration.m_translation);
		//m_object.setOrientation(toQuaternion(configuration.m_eulerAngles));
		m_object.setOrientation(configuration.m_orientation);
		//m_object.setOrientation(configuration.m_eulerAngles[0], configuration.m_eulerAngles[1], configuration.m_eulerAngles[2]);
		return m_collisionManager->doCollide();
	}

	float SixDofPlannerBase::configurationDistance(const Configuration & c1, const Configuration & c2) 
	{
		float wR = 1;
		float wT = 1;
		return (c1.m_translation-c2.m_translation).norm()*wT + wR*acos(c1.m_orientation.dot(c2.m_orientation)) /(0.5*Math::pi); // For  compilation purpose
	}

	SixDofPlannerBase::Configuration SixDofPlannerBase::limitDistance(const Configuration& source, const Configuration& target, float maxDistance, size_t iterationLimit)
	{
		std::pair<float, float> interval = {0.0f, 1.0f};
		if (configurationDistance(source, target) <= maxDistance) { return target; }
		for (size_t cpt = 0; cpt < iterationLimit; ++cpt)
		{
			float middle = (interval.first + interval.second) * 0.5f;
			if (configurationDistance(source, source.interpolate(target, middle)) < maxDistance)
			{
				interval.first = middle;
			}
			else
			{
				interval.second = middle;
			}
		}
		Configuration result = source.interpolate(target, interval.first);
		return result;
	}

	bool SixDofPlannerBase::doCollide(const Configuration & start, const Configuration & end, float dq) const
	{
		float d = configurationDistance(start, end);
		if (d < dq) {
			return false;
		}
		else {
			/*
			Math::Quaternion<float> mid_orientation = Math::Quaternion<float>::slerp(start.m_orientation, end.m_orientation, 0.5);
			Math::Vector3f mid_translation = (start.m_translation + end.m_translation) / 2;
			*/
			Configuration mid= start.interpolate(end,0.5);
			if (doCollide(mid)) {
				return true;
			}
			else {
				return doCollide(start, mid, dq) || doCollide(mid, end, dq);
			}
		}
	}

	void SixDofPlannerBase::optimize(::std::vector<Configuration>& toOptimize, float dq) const
	{
		// Does nothing for now...
	}
}