#pragma once

#include <AI/Blackboard.h>
#include <Math/Vectorf.h>
#include <Math/Constant.h>
#include <Math/Interval.h>
#include <stdext/message_handler.h>
#include <vector>
#include <memory>

namespace Crowds
{
	class Simulator;

	/// <summary>
	/// Abstract base class for agents
	/// </summary>
	class Agent
	{
	public:

		enum class Status { 
			/// <summary> The agent is running </summary>
			running, 
			/// <summary> The agent execution is stopped </summary>
			stopped
		};

	private:
		friend class Simulator;

		Simulator * m_simulator;

	private:
		AI::Blackboard * m_blackboard;
		Math::Vector2f m_position;
		Math::Vector2f m_speed;
		float m_orientation;
		float m_radius;
		Status m_status;

		/// <summary>
		/// Updates the orientation of the agent.
		/// </summary>
		void updateOrientation()
		{
			if (m_speed.norm() != 0.0)
			{
				m_orientation = atan2(m_speed[1], m_speed[0]);
			}
		}

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="Agent"/> class.
		/// </summary>
		/// <param name="simulator">The simulator.</param>
		Agent(Simulator * simulator, const Math::Vector2f & position, float radius)
			: m_simulator(simulator), m_position(position), m_speed(0.0), m_orientation(Math::Interval<float>(-Math::pi, Math::pi).random()), m_radius(radius),
			m_blackboard(new AI::Blackboard), m_status(Status::running)
		{}

		/// <summary>
		/// Gets the agent's status.
		/// </summary>
		/// <returns></returns>
		Status getStatus() const { return m_status; }

		/// <summary>
		/// Sets the agent status.
		/// </summary>
		/// <param name="status">The status.</param>
		void setStatus(Status status) { m_status = status; }

		/// <summary>
		/// Gets the simulator.
		/// </summary>
		/// <returns></returns>
		Simulator * getSimulator() const { return m_simulator; }

		/// <summary>
		/// Gets the agent's blackboard.
		/// </summary>
		/// <returns></returns>
		AI::Blackboard * getBlackboard() const { return m_blackboard; }

		/// <summary>
		/// Gets the agent position.
		/// </summary>
		/// <returns></returns>
		const Math::Vector2f & getPosition() const { return m_position; }

		/// <summary>
		/// Sets the position.
		/// </summary>
		/// <param name="position">The position.</param>
		void setPosition(const Math::Vector2f & position) { m_position = position; }

		/// <summary>
		/// Gets the radius.
		/// </summary>
		/// <returns></returns>
		float getRadius() const { return m_radius; }

		/// <summary>
		/// Gets the speed.
		/// </summary>
		/// <returns></returns>
		const Math::Vector2f & getSpeed() const { return m_speed; }

		/// <summary>
		/// Sets the speed.
		/// </summary>
		/// <param name="speed">The speed.</param>
		void setSpeed(const Math::Vector2f & speed)
		{
			m_speed = speed;
			updateOrientation();
		}

		/// <summary>
		/// Gets the orientation of the Agent.
		/// </summary>
		/// <returns></returns>
		float getOrientation() const { return m_orientation; }

		/// <summary>
		/// Gets the vector indicating the front direction.
		/// </summary>
		/// <returns></returns>
		Math::Vector2f getFrontVector() const { return Math::makeVector(cos(m_orientation), sin(m_orientation)); }

		/// <summary>
		/// Perceives the entities of type EntityType beeing at a distance lesser than radius and in a field of view of angle 2*openingAngle.
		/// </summary>
		/// <param name="radius">The maixmum distance of perception.</param>
		/// <param name="openingAngle">The half opening angle of the field of view.</param>
		/// <returns></returns>
		template <typename EntityType>
		std::vector<std::shared_ptr<EntityType>> perceive(float radius, float openingAngle = Math::pi)
		{
			std::vector<std::shared_ptr<EntityType>>neighbours = getSimulator()->selectEntities<EntityType>(getPosition(), radius);
			Math::Vector2f front = getFrontVector();
			//for (auto it = neighbours.begin(); it != neighbours.end();)
			for(size_t cpt=0; cpt<neighbours.size() ; )
			{
				auto it = neighbours.begin() + cpt;
				if (((*it).get() == this) || ((*it)->getPosition() - getPosition()).normalized()*front < cos(openingAngle))
				{
					(*it) = neighbours.back();
					neighbours.pop_back();
				}
				else
				{
					++cpt;
				}
			}
			return neighbours;
		}

		/// <summary>
		/// Removes all the agents for which the condition is true in the provided vector.
		/// </summary>
		/// <param name="toFilter">The vector that should be filtered.</param>
		/// <param name="condition">The condition.</param>
		template <typename AgentType, typename Condition>
		static void removeIf(std::vector<std::shared_ptr<AgentType>> & toFilter, const Condition & condition)
		{
			static_assert(std::is_base_of<Agent, AgentType>::value, "The provided type AgentType should be inheriting from Agent");
			auto it = std::remove_if(toFilter.begin(), toFilter.end(), condition);
			toFilter.erase(it, toFilter.end());
		}

		/// <summary>
		/// Filters the agents in the provided vector by type (uses dynamic cast).
		/// </summary>
		/// <param name="source">The source vector.</param>
		/// <returns> The filtered vector. </returns>
		template <typename TargetType, typename AgentType>
		static std::vector<std::shared_ptr<TargetType>> filterByType(const std::vector<std::shared_ptr<AgentType>> & source)
		{
			static_assert(std::is_base_of<Agent, TargetType>::value, "The provided type TargetType should be inheriting from Agent");
			static_assert(std::is_base_of<Agent, AgentType>::value, "The provided type AgentTYpe should be inheriting from agent");
			std::vector<std::shared_ptr<TargetType>> result;
			result.reserve(source.size());
			for (auto it = source.begin(), end = source.end(); it != end; ++it)
			{
				std::shared_ptr<TargetType> current = std::dynamic_pointer_cast<TargetType>(*it);
				if (current)
				{
					result.push_back(current);
				}
			}
			return result;
		}

		/// <summary>
		/// Broadcasts the specified message to the world.
		/// </summary>
		/// <param name="message">The message to broadcast.</param>
		template <typename MessageType>
		void broadcast(const MessageType & message)
		{
			getSimulator()->broadcast(message);
		}

		/// <summary>
		/// Posts the specified message to the specified agent.
		/// </summary>
		/// <param name="message">The message.</param>
		/// <param name="agent">The agent.</param>
		template <typename MessageType>
		void post(const MessageType & message, Agent * agent)
		{
			getSimulator()->post(message, agent);
		}

		/// <summary>
		/// Posts the specified message to the entities lying in the provided circle.
		/// </summary>
		/// <param name="message">The message.</param>
		/// <param name="position">The position.</param>
		/// <param name="radius">The radius.</param>
		template <typename MessageType>
		void post(const MessageType & message, float radius)
		{
			getSimulator()->post(message, getPosition(), radius);
		}

		/// <summary>
		/// Creates the receiver for the provided message type. Once a message of this type is received, the callback function is called
		/// with the provided message as a parameter.
		/// </summary>
		/// <param name="callback">The callback function. Signature : void function(const MessageType & message)</param>
		/// <returns></returns>
		template <typename MessageType, typename Callback>
		stdext::message_handler::receiver_callback<MessageType> * createReceiver(const Callback & callback)
		{
			return getSimulator()->createReceiver<MessageType>(callback, this);
		}

		/// <summary>
		/// Updates the agent for the provided time step.
		/// </summary>
		/// <param name="dt">The dt.</param>
		virtual void update(double dt) = 0;

		/// <summary>
		/// Finalizes an instance of the <see cref="Agent"/> class.
		/// </summary>
		virtual ~Agent()
		{
			delete m_blackboard;
		}
	};
}