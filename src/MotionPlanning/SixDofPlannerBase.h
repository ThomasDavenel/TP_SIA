#pragma once
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
	class SixDofPlannerBase
	{
	public:

		/// <summary>
		/// Converts Euler angles into a quaternion
		/// </summary>
		/// <param name="eulerAngles">The euler angles.</param>
		/// <returns></returns>
		static Math::Quaternion<float> toQuaternion(const Math::Vector3f & eulerAngles)
		{
			Math::Vector3f xAxis = Math::makeVector(1.f, 0.f, 0.f);
			Math::Vector3f yAxis = Math::makeVector(0.f, 1.f, 0.f);
			Math::Vector3f zAxis = Math::makeVector(0.f, 0.f, 1.f);
			float xAngle = eulerAngles[0];
			float yAngle = eulerAngles[1];
			float zAngle = eulerAngles[2];
			return (Math::Quaternion<float>(xAxis, xAngle)*Math::Quaternion<float>(yAxis, yAngle)*Math::Quaternion<float>(zAxis, zAngle));
		}

		/// <summary>
		/// A configuration
		/// </summary>
		struct Configuration
		{
			/// <summary>
			/// The translation
			/// </summary>
			Math::Vector3f m_translation;
			/// <summary>
			/// The orientation
			/// </summary>
			Math::Quaternion<float> m_orientation;

			/// <summary>
			/// Equality comparison.
			/// </summary>
			/// <param name="other">The other.</param>
			/// <returns></returns>
			bool operator==(const Configuration & other) const
			{
				return m_translation == other.m_translation && m_orientation == other.m_orientation;
			}

			/// <summary>
			/// Initializes a new instance of the <see cref="Configuration"/> struct.
			/// </summary>
			Configuration()
			{}

			/// <summary>
			/// Initializes a new instance of the <see cref="Configuration"/> struct.
			/// </summary>
			/// <param name="translation">The translation.</param>
			/// <param name="eulerAngles">The euler angles providing the orientation of the mobile.</param>
			Configuration(const Math::Vector3f & translation, const Math::Vector3f & eulerAngles)
				: m_translation(translation), m_orientation(toQuaternion(eulerAngles))
			{}

			/// <summary>
			/// Initializes a new instance of the <see cref="Configuration"/> struct.
			/// </summary>
			/// <param name="translation">The translation.</param>
			/// <param name="orientation">The orientation of the mobile.</param>
			Configuration(const Math::Vector3f & translation, const Math::Quaternion<float> & orientation)
				: m_translation(translation), m_orientation(orientation)
			{}

			/// <summary>
			/// Interpolates between this configuration and the provided one.
			/// </summary>
			/// <param name="target">The target configuration.</param>
			/// <param name="t">The interpolation parameter in interval [0;1].</param>
			/// <returns></returns>
			Configuration interpolate(const Configuration & target, float t) const
			{
				return Configuration(Math::Interpolation::lerp(m_translation, target.m_translation, t), Math::Quaternion<float>::slerp(m_orientation, target.m_orientation, t));
			}
		};

		/// <summary>
		/// The default search interval for angles.
		/// </summary>
		/// <returns></returns>
		static std::pair<float, float> defaultAngleInterval()
		{
			return std::make_pair(-float(Math::pi), float(Math::pi));
		}

		/// <summary>
		/// The default search interval for positions.
		/// </summary>
		/// <returns></returns>
		static std::pair<float, float> defaultPositionInterval()
		{
			return std::make_pair(-1.0f, 1.0f);
		}

	protected:
		/// <summary>
		/// A uniform random generator
		/// </summary>
		Math::UniformRandom m_uniformRandom;

	private:
		/// <summary>
		/// The search intervals (x, y, z, angle X, angle Y, angle Z)
		/// </summary>
		std::vector<std::pair<float, float>> m_intervals;
		/// <summary>
		/// The collision manager
		/// </summary>
		MotionPlanning::CollisionManager * m_collisionManager;
		/// <summary>
		/// The mobile
		/// </summary>
		mutable MotionPlanning::CollisionManager::DynamicCollisionObject m_object;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="SixDofPlannerBase"/> class.
		/// </summary>
		/// <param name="collisionManager">The collision manager.</param>
		/// <param name="object">The mobile.</param>
		/// <param name="maxDistance">The maximum distance between two samples along an interpolation path.</param>
		/// <param name="intervals">The search intervals (x, y, z, angle X, angle Y, angle Z).</param>
		SixDofPlannerBase(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::initializer_list<std::pair<float, float>>& intervals = 
			{ defaultPositionInterval(), defaultPositionInterval(), defaultPositionInterval(), defaultAngleInterval(), defaultAngleInterval(), defaultAngleInterval() });

		/// <summary>
		/// Initializes a new instance of the <see cref="SixDofPlannerBase"/> class.
		/// </summary>
		/// <param name="collisionManager">The collision manager.</param>
		/// <param name="object">The mobile.</param>
		/// <param name="maxDistance">The maximum distance between two samples along an interpolation path.</param>
		/// <param name="intervals">The search intervals (x, y, z, angle X, angle Y, angle Z).</param>
		SixDofPlannerBase(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::vector<std::pair<float, float>>& intervals);

		/// <summary>
		/// Generates a random configuration.
		/// </summary>
		/// <returns></returns>
		Configuration randomConfiguration() const;

		/// <summary>
		/// Computes the minimum distance between the mobile and the obstacles.
		/// </summary>
		/// <param name="configuration">The configuration.</param>
		/// <returns></returns>
		float distanceToObstacles(const Configuration & configuration) const;

		/// <summary>
		/// Tests if this configuration is in collision with the environment
		/// </summary>
		/// <param name="configuration">The configuration.</param>
		/// <returns></returns>
		bool doCollide(const Configuration & configuration) const;

		/// <summary>
		/// Tests if the interpolation between two configurations collides with the environment.
		/// </summary>
		/// <param name="start">The start configuration.</param>
		/// <param name="end">The target configuration.</param>
		/// <param name="dq">The maximum distance between two samples along the path.</param>
		/// <returns></returns>
		bool doCollide(const Configuration & start, const Configuration & end, float dq) const;

		/// <summary>
		/// Optimizes the specified path.
		/// </summary>
		/// <param name="toOptimize">The path to optimize.</param>
		/// <param name="dq">The maximum distance between to samples along the path.</param>
		void optimize(::std::vector<Configuration> & toOptimize, float dq) const;

		/// <summary>
		/// Distance between two configurations
		/// </summary>
		/// <param name="c1">The c1.</param>
		/// <param name="c2">The c2.</param>
		/// <returns></returns>
		static float configurationDistance(const Configuration & c1, const Configuration & c2);

		/// <summary>
		/// Finds a configuration between source and target such as the distance between the source and the configuration is as close as possible to maxDistance and never greater than the distance between source and target.
		/// </summary>
		/// <param name="source">The source.</param>
		/// <param name="target">The target.</param>
		/// <param name="maxDistance">The maximum distance.</param>
		/// <param name="iterationLimit">The iteration limit when approximating the best solution.</param>
		/// <returns></returns>
		static Configuration limitDistance(const Configuration& source, const Configuration& target, float maxDistance, size_t iterationLimit = 32);
		
		virtual bool plan(const Configuration & start, const Configuration & target, float radius, float dq, std::vector<Configuration> & result) = 0;
	};
}