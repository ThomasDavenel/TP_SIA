#pragma once

#include <Crowds/Agent.h>
#include <Crowds/Simulator.h>
#include <Math/Polynomial2.h>
#include <Math/Vectorf.h>
#include <Math/Sampler.h>
#include <Math/PolarCoordinates.h>

namespace Crowds
{
	/// <summary>
	/// A boid agent using Reynolds basic model (integration on a point mass system)
	/// </summary>
	/// <seealso cref="LocalizedAgent2d" />
	class Boid : public Agent
	{
	protected:
		Math::Vector2f m_acceleration;
		Math::Vector2f m_steeringForce;
		float m_mass;
		float m_maxSpeed;
		float m_maxForce;

		/// <summary>
		/// Truncates the specified vector such as is norm is no more that maxNorm.
		/// </summary>
		/// <param name="vector">The vector.</param>
		/// <param name="maxNorm">The maximum norm.</param>
		/// <returns></returns>
		static Math::Vector2f truncate(const Math::Vector2f & vector, float maxNorm)
		{
			if (vector.norm() > maxNorm)
			{
				return vector.normalized()*maxNorm;
			}
			return vector;
		}

		/// <summary>
		/// Integrates the agent speed and position given the specified dt.
		/// </summary>
		/// <param name="dt">The dt (integration step).</param>
		void integrate(double dt)
		{
			m_acceleration = truncate(m_steeringForce, m_maxForce) / m_mass;
			setSpeed(truncate(getSpeed() + m_acceleration * dt, m_maxSpeed));
			setPosition(getPosition() + getSpeed() * dt);
		}

		/// <summary>
		/// Struct containing propositions for varying the speed during the collision avoidance.
		/// </summary>
		struct SpeedVariation
		{
			float m_cost;
			float m_angleVariation;
			float m_speedPercentage;

			bool operator<(const SpeedVariation & other) const
			{
				return m_cost < other.m_cost;
			}
		};

		/// <summary>
		/// Gets the speed table.
		/// </summary>
		/// <returns></returns>
		static const std::vector<SpeedVariation> & getSpeedTable()
		{
			static std::vector<SpeedVariation> speeds;
			float step = 0.2;
			if (speeds.size() == 0)
			{
				for (float angle = -Math::pi; angle <= Math::pi; angle += step)
				{
					for (float percentage = 0.0; percentage <= 2.0; percentage += step)
					{
						float cost = pow(fabs(angle) / Math::pi, 2) + pow(percentage - 1.0, 2);
						speeds.push_back(SpeedVariation{ cost, angle, percentage });
					}
				}
				std::sort(speeds.begin(), speeds.end());
			}
			return speeds;
		}

		/// <summary>
		/// Computes the collision polynomial.
		/// </summary>
		/// <param name="position1">The position1.</param>
		/// <param name="speed1">The speed1.</param>
		/// <param name="radius1">The radius1.</param>
		/// <param name="position2">The position2.</param>
		/// <param name="speed2">The speed2.</param>
		/// <param name="radius2">The radius2.</param>
		/// <returns></returns>
		static Math::Polynomial2<float> computeCollisionPolynomial(const Math::Vector2f & position1, const Math::Vector2f speed1, float radius1, const Math::Vector2f & position2, const Math::Vector2f & speed2, float radius2)
		{
			Math::Vector2f deltaP = position1 - position2;
			Math::Vector2f deltaV = speed1 - speed2;
			float sumR = radius1 + radius2;
			return Math::Polynomial2<float>(deltaV*deltaV, deltaP*deltaV*2.0, deltaP*deltaP - sumR * sumR);
		}

		/// <summary>
		/// Computes the collision polynomial representing the squared distance between two moving agents.
		/// </summary>
		/// <param name="other">The other agent.</param>
		/// <returns></returns>
		Math::Polynomial2<float> computeCollisionPolynomial(const Agent & other) const
		{
			Math::Vector2f deltaP = getPosition() - other.getPosition();
			Math::Vector2f deltaV = getSpeed() - other.getSpeed();
			float sumR = getRadius() + other.getRadius();
			Math::Polynomial2<float> polynomial(deltaV*deltaV, deltaP*deltaV*2.0, deltaP*deltaP - sumR * sumR);
			return polynomial;
		}

		/// <summary>
		/// Determines whether this instance can collide with the other instance during the provided time interval.
		/// </summary>
		/// <param name="other">The other.</param>
		/// <param name="timeInterval">The time interval.</param>
		/// <returns>
		///   <c>true</c> if this instance can collide the other one; otherwise, <c>false</c>.
		/// </returns>
		bool canCollide(const Agent & other, const Math::Interval<float> & timeInterval, float & collisionTime) const
		{
			Math::Polynomial2<float> polynomial = computeCollisionPolynomial(other);
			float miniRoot = polynomial.minimumRoot(timeInterval);
			float value = polynomial(miniRoot);
			bool result = value <= 0.0;
			if (result)
			{
				std::array<float, 2> solutions = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
				int nb = polynomial.solutions(solutions);
				if (nb == 2)
				{
					if (solutions[0] >= 0.0) { collisionTime = solutions[0]; }
					else { collisionTime = solutions[1]; }
				}
				else if (nb == 1)
				{
					collisionTime = solutions[0];
				}
			}
			return result;
		}

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="Boid"/> class.
		/// </summary>
		/// <param name="simulator">The simulator.</param>
		/// <param name="position">The initial position.</param>
		Boid(Simulator * simulator, const Math::Vector2f & position, float radius, float mass, float maxSpeed, float maxForce)
			: Agent(simulator, position, radius), m_mass(mass), m_acceleration(0.0f), m_steeringForce(0.0f), m_maxSpeed(maxSpeed), m_maxForce(maxForce)
		{}

		/// <summary>
		/// Gets the maximum force.
		/// </summary>
		/// <returns></returns>
		float getMaxForce() const { return m_maxForce; }

		/// <summary>
		/// Sets the maximum force.
		/// </summary>
		/// <param name="maxForce">The maximum force.</param>
		void setMaxForce(float maxForce) { m_maxForce = maxForce; }

		/// <summary>
		/// Gets the maximum speed.
		/// </summary>
		/// <returns></returns>
		float getMaxSpeed() const { return m_maxSpeed; }

		/// <summary>
		/// Sets the maximum speed.
		/// </summary>
		/// <param name="maxSpeed">The maximum speed.</param>
		void setMaxSpeed(float maxSpeed) { m_maxSpeed = maxSpeed; }

		/// <summary>
		/// Gets the acceleration.
		/// </summary>
		/// <returns></returns>
		const Math::Vector2f & getAcceleration() const { return m_acceleration; }

		/// <summary>
		/// Sets the steering force.
		/// </summary>
		/// <param name="steeringForce">The steer force.</param>
		void setSteeringForce(const Math::Vector2f & steeringForce) { m_steeringForce = steeringForce; }

		/// <summary>
		/// Adds the steering force to the current steering force.
		/// </summary>
		/// <param name="sterringForce">The sterring force.</param>
		void addSteeringForce(const Math::Vector2f & sterringForce) { m_steeringForce += sterringForce; }

		/// <summary>
		/// Gets the steering force.
		/// </summary>
		/// <returns></returns>
		const Math::Vector2f & getSteeringForce() const { return m_steeringForce; }

		/// <summary>
		/// Gets the mass of the agent.
		/// </summary>
		/// <returns></returns>
		float getMass() const { return m_mass; }

		/// <summary>
		/// Updates the agent (position and speed) and resets the steering force for next computations.
		/// The update method used by inheriting agents should call this implementation at the end of its own update.
		/// </summary>
		/// <param name="dt">The dt.</param>
		virtual void update(double dt) override
		{
			integrate(dt); // We compute the new position and speed
			m_steeringForce = Math::Vector2f(0.0); // We reset the steering force
		}

		/// <summary>
		/// Seeks the specified target.
		/// </summary>
		/// <param name="target">The target.</param>
		/// <returns></returns>
		Math::Vector2f seek(const Math::Vector2f & target, float adaptationTime = 1.0f) const
		{
			return ((target - getPosition()).normalized()*m_maxSpeed - getSpeed()) / adaptationTime;
		}

		/// <summary>
		/// Flees the specified position.
		/// </summary>
		/// <param name="position">The position.</param>
		/// <returns></returns>
		Math::Vector2f flee(const Math::Vector2f & position, float adpatationTime = 1.0f) const
		{
			return (-((position - getPosition()).normalized()*m_maxSpeed) - getSpeed()) / adpatationTime;
		}

		/// <summary>
		/// Pursuits the specified BOID.
		/// </summary>
		/// <param name="boid">The BOID.</param>
		/// <param name="extrapolationTime">The extrapolation time.</param>
		/// <returns></returns>
		Math::Vector2f pursuit(const Agent & boid, float extrapolationTime, float adaptationTime = 1.0f)
		{
			return seek(boid.getPosition() + boid.getSpeed() * extrapolationTime, adaptationTime);
		}

		/// <summary>
		/// Try to evades the specified BOID.
		/// </summary>
		/// <param name="boid">The BOID.</param>
		/// <param name="extrapolationTime">The extrapolation time.</param>
		/// <returns></returns>
		Math::Vector2f evasion(const Agent & boid, float extrapolationTime, float adaptationTime = 1.0f)
		{
			return flee(boid.getPosition() + boid.getSpeed()*extrapolationTime, adaptationTime);
		}

		/// <summary>
		/// Arrives at the specified target.
		/// </summary>
		/// <param name="target">The target.</param>
		/// <param name="slowingDistance">The slowing distance.</param>
		/// <returns></returns>
		Math::Vector2f arrival(const Math::Vector2f & target, float slowingDistance, float adaptationTime=1.0f)
		{
			Math::Vector2f target_offset = target - getPosition();
			float distance = target_offset.norm();
			float ramped_speed = m_maxSpeed * (distance / slowingDistance);
			float clipped_speed = std::min(ramped_speed, m_maxSpeed);
			Math::Vector2f desired_velocity = target_offset * (clipped_speed / distance);
			return (desired_velocity - getSpeed())/adaptationTime;
		}

		/// <summary>
		/// Asks the boid to stop moving.
		/// </summary>
		/// <param name="adaptationTime">The adaptation time.</param>
		/// <returns></returns>
		Math::Vector2f stop(float adaptationTime = 1.0f)
		{
			return getSpeed()/(-adaptationTime);
		}

		/// <summary>
		/// Wanders.
		/// </summary>
		/// <param name="circleDistance">The circle distance.</param>
		/// <param name="circleRadius">The circle radius.</param>
		/// <param name="previousAngle">The previous angle.</param>
		/// <param name="modificationPercentage">The modification percentage.</param>
		/// <returns></returns>
		Math::Vector2f wander(float circleDistance, float circleRadius, float & previousAngle, float modificationPercentage, float adaptationTime = 1.0f)
		{
			Math::Vector2f direction = getFrontVector();
			//if (m_speed.norm() != 0.0) { direction = m_speed.normalized(); }
			//else { direction = Math::makeVector(1.0, 0.0); }
			Math::Vector2f center = direction * circleDistance;
			float delta = (Math::Sampler::random() - 0.5f)*2.0f*modificationPercentage*Math::pi;
			previousAngle += delta;
			Math::Vector2f newSpeed = (Math::makeVector(cos(previousAngle), sin(previousAngle))*circleRadius + center).normalized()*m_maxSpeed;
			return (newSpeed - getSpeed()) / adaptationTime;
		}

		/// <summary>
		/// Cohesion steering force.
		/// </summary>
		/// <param name="neighbours">The neighbours.</param>
		/// <returns></returns>
		template <typename AgentType>
		Math::Vector2f cohesion(const std::vector<std::shared_ptr<AgentType>> & neighbours, float adaptationTime = 1.0f) const
		{
			static_assert(std::is_base_of<Agent, AgentType>::value, "AgentType must inherit from agent");
			Math::Vector2f center(0.0);
			for (auto it = neighbours.begin(), end = neighbours.end(); it != end; ++it)
			{
				const std::shared_ptr<Agent> & current = (*it);
				if (current.get() == this) { continue; }
				center = center + current->getPosition();
			}
			center = center / (neighbours.size() - 1);
			return seek(center, adaptationTime);
		}

		/// <summary>
		/// Cohesion steering force.
		/// </summary>
		/// <param name="perceptionRadius">The perception radius.</param>
		/// <returns></returns>
		Math::Vector2f cohesion(float perceptionRadius, float adaptationTime = 1.0f) const
		{
			std::vector<std::shared_ptr<Agent>> neighbours = getSimulator()->selectEntities<Agent>(getPosition(), perceptionRadius);
			return cohesion(neighbours, adaptationTime);
		}

		/// <summary>
		/// Alignment steering force.
		/// </summary>
		/// <param name="neighbours">The neighbours.</param>
		/// <returns></returns>
		template <typename AgentType>
		Math::Vector2f alignment(const std::vector<std::shared_ptr<AgentType>> & neighbours, float adaptationTime = 1.0f) const
		{
			static_assert(std::is_base_of<Agent, AgentType>::value, "AgentType must inherit from agent");
			Math::Vector2f average(0.0);
			for (auto it = neighbours.begin(), end = neighbours.end(); it != end; ++it)
			{
				const std::shared_ptr<Agent> & current = (*it);
				average = average + (*it)->getSpeed();
			}
			average = average / neighbours.size();
			return (average - getSpeed()) / adaptationTime;
		}

		/// <summary>
		/// Alignment steering force.
		/// </summary>
		/// <param name="perceptionRadius">The perception radius.</param>
		/// <returns></returns>
		Math::Vector2f alignment(float perceptionRadius, float adaptationTime = 1.0f) const
		{
			std::vector<std::shared_ptr<Agent>> neighbours = getSimulator()->selectEntities<Agent>(getPosition(), perceptionRadius);
			return alignment(neighbours, adaptationTime);
		}

		/// <summary>
		/// Separation steering force.
		/// </summary>
		/// <param name="neighbours">The neighbours.</param>
		/// <returns></returns>
		template <typename AgentType>
		Math::Vector2f separation(const std::vector<std::shared_ptr<AgentType>> & neighbours, float adaptationTime = 1.0f)
		{
			static_assert(std::is_base_of<Agent, AgentType>::value, "AgentType must inherit from agent");
			Math::Vector2f average(0.0);
			Math::Vector2f force(0.0);
			for (auto it = neighbours.begin(), end = neighbours.end(); it != end; ++it)
			{
				const std::shared_ptr<Agent> & current = (*it);
				if (current.get() == this) { continue; }
				Math::Vector2f delta = current->getPosition() - getPosition();
				force = force - delta.normalized() / (std::max(delta.norm()-getRadius()-current->getRadius(), 0.01f));
			}
			return force / adaptationTime;
		}

		/// <summary>
		/// Separation steering force.
		/// </summary>
		/// <param name="perceptionRadius">The perception radius.</param>
		/// <returns></returns>
		Math::Vector2f separation(float perceptionRadius, float adaptationTime = 1.0f)
		{
			std::vector<std::shared_ptr<Agent>> neighbours = getSimulator()->selectEntities<Agent>(getPosition(), perceptionRadius);
			return separation(neighbours, adaptationTime);
		}

		/// <summary>
		/// Tests if that agent may collide with at least one of the provided ones whitin the provided time window
		/// </summary>
		/// <param name="neighbours">The neighbours.</param>
		/// <param name="timeWindow">The time window.</param>
		/// <returns></returns>
		template <typename AgentType>
		bool mayCollide(std::vector<std::shared_ptr<AgentType>> & neighbours, const Math::Interval<float> & timeWindow = Math::makeInterval(0.0f, 10.0f)) const
		{
			// We search for the collision which is the closest in time.
			for (auto it = neighbours.begin(), end = neighbours.end(); it != end; ++it)
			{
				const std::shared_ptr<Agent> & other = (*it);
				float collisionTime = std::numeric_limits<float>::max();
				if (canCollide(*other, timeWindow, collisionTime))
				{
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// The collision avoidance behavior. This behavior tries to find a speed that avoids collisions with all the neighbours. 
		/// If this behavior returns a null force, this means that no potential collision has been found.
		/// </summary>
		/// <param name="m_neighbours">The neighbours are in/out but only their order might be changed (for optimisation reasons).</param>
		/// <param name="adaptationTime">The adaptation time.</param>
		/// <param name="timeWindow">The time window for collision prediction.</param>
		/// <returns></returns>
		template <typename AgentType>
		Math::Vector2f avoidCollisions(std::vector<std::shared_ptr<AgentType>> & neighbours, float adaptationTime = 1.0, const Math::Interval<float> & timeWindow = Math::makeInterval(0.0f, 10.0f))
		{
			bool collision = false; // Is there a predicted collision ? 
			float minCollisionTime = std::numeric_limits<float>::max(); // When does this collision happen
			std::shared_ptr<Agent> nearest; // Who is the collider

			// We search for the collision which is the closest in time.
			for (auto it = neighbours.begin(), end = neighbours.end(); it != end; ++it)
			{
				const std::shared_ptr<Agent> & other = (*it);
				float collisionTime = std::numeric_limits<float>::max();
				bool col = canCollide(*other, timeWindow, collisionTime);
				collision |= col;
				if (col && minCollisionTime > collisionTime)
				{
					minCollisionTime = collisionTime;
					nearest = other;
				}
			}
			// If a collision has been detected
			if (nearest)
			{
				if ((getPosition() - nearest->getPosition()).norm() <= getRadius() + nearest->getRadius())
				{	// Agents are colliding...
					getSimulator()->broadcast(CollisionMessage{ this, nearest.get() });
					//addSteeringForce(separation(m_neighbours, m_dt));
					return separation(neighbours, adaptationTime);
				}
				else
				{	// Agents are not colliding
					auto computePolynomial = [this](const Math::Vector2f & speed, const Agent & other)
					{
						return computeCollisionPolynomial(getPosition(), speed, getRadius(), other.getPosition(), other.getSpeed(), other.getRadius());
					};
					// We prepare the solving algorithm
					Math::PolarCoordinates polarSpeed(getSpeed());
					const std::vector<SpeedVariation> & speeds = getSpeedTable();
					bool foundSolution = false;
					// We look for a speed that avoids collisions with all neighbours
					for (auto it = speeds.begin(), end = speeds.end(); it != end; ++it)
					{
						Math::PolarCoordinates testedSpeed = polarSpeed.scale(it->m_speedPercentage).rotate(it->m_angleVariation);
						bool collisionFound = false;
						// We validate or invalidate the proposed speed
						for (auto neighbourIt = neighbours.begin(), end = neighbours.end(); neighbourIt != end; ++neighbourIt)
						{
							float t;
							Math::Polynomial2<float> currentPoly = computePolynomial(testedSpeed, **neighbourIt);
							if (currentPoly(currentPoly.minimumRoot(timeWindow)) <= 0.1) // Collision case (we use a margin of 0.1m)
							{
								if (neighbourIt != neighbours.begin())
								{   // In dense situations, it accelerates the computation
									std::swap((*neighbourIt), *(neighbourIt - 1));
								}
								collisionFound = true;
								break;
							}
						}
						if (!collisionFound) // If we did not find a collision in the previous loop, the speed is a solution to the avoidance problem
						{
							foundSolution = true;
							polarSpeed = testedSpeed;
							break;
						}
					}
					if (foundSolution) // If we found a solution, we apply it
					{
						Math::Vector2f newSpeed = polarSpeed;
						return (newSpeed - getSpeed()) / adaptationTime;
					}
					else // No solution has been found. The agent stops and tries to get as far as possible from its neighbours.
					{
						return (-getSpeed() / adaptationTime) + separation(neighbours, adaptationTime);
					}
				}
			}
			return Math::makeVector(0.0f, 0.0f); // Default, no collision to avoid so no contribution.
		}
	};
}