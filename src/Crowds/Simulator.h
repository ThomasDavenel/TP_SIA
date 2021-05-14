#pragma once

#include <Crowds/Agent.h>
#include <vector>
#include <memory>
#include <stdext/message_handler.h>
#include <tbb/task_group.h>

namespace Crowds
{
	/// <summary>
	/// The simulator of 2D agents navigating in an environment. This class simulates all the agents and provided
	/// a neighborhood data structure accelerating neighbourhood requests.
	/// </summary>
	class Simulator : public stdext::message_handler
	{
	protected:
		std::vector<std::shared_ptr<Agent>> m_agents;
		double m_time;
		MotionPlanning::VPTree<std::shared_ptr<Agent>, Math::Vector2f> m_neighbourhoodTree;
		//stdext::message_handler m_messageHandler;
		bool m_needsInitialisation;
		tbb::task_group * m_tasksGroup;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="Simulator"/> class.
		/// </summary>
		Simulator()
			: m_time(0), m_neighbourhoodTree(
				[](const std::shared_ptr<Agent> & a1, const std::shared_ptr<Agent> & a2)
				{
					return (a1->getPosition() - a2->getPosition()).norm();
				},
				[](const std::shared_ptr<Agent> & agent, const Math::Vector2f & position)
				{
					return (agent->getPosition() - position).norm();
				}
			),
			m_needsInitialisation(true)
		{
			m_tasksGroup = new tbb::task_group;
		}

		/// <summary>
		/// Finalizes an instance of the <see cref="Simulator"/> class.
		/// </summary>
		virtual ~Simulator() 
		{
			try
			{
				delete m_tasksGroup;
			}
			catch (const std::exception & e)
			{
				std::cout << "Exception raised: " << e.what() << std::endl;
			}
		}

		/// <summary>
		/// Gets the message handler. This message can be used to communicate between different agents.
		/// </summary>
		/// <returns></returns>
		//stdext::message_handler & getMessageHandler() { return m_messageHandler; }

		/// <summary>
		/// Gets the absolute time.
		/// </summary>
		/// <returns></returns>
		double getTime() const { return m_time; }

		/// <summary>
		/// Updates all agents.
		/// </summary>
		/// <param name="dt">The dt.</param>
		virtual void update(double dt)
		{
			//std::cout << "Number of simulated agents: " << m_agents.size() << ", in tree " << m_neighbourhoodTree.size() << std::endl;
			if (m_needsInitialisation)
			{
				m_neighbourhoodTree.recompute();
				m_needsInitialisation = false;
			}
			m_tasksGroup->wait();
			for (auto it = m_agents.begin(), end = m_agents.end(); it != end; ++it)
			{
				if ((*it)->getStatus() == Agent::Status::running)
				{
					(*it)->update(dt);
				}
			}
			auto updateNeighourhood = [this]() { m_neighbourhoodTree.recompute(); };
			m_tasksGroup->run(updateNeighourhood);
			m_time += dt;
		}

		/// <summary>
		/// Creates a new agent of the provided type.
		/// </summary>
		/// <param name="...params">The parameters to forward to the AgentType constructor.</param>
		/// <returns></returns>
		template <typename AgentType, typename... Parameters>
		std::shared_ptr<AgentType> createAgent(Parameters... params)
		{
			m_tasksGroup->wait(); // We  cannot create an agent while the data structure is updated
			static_assert(std::is_base_of<Agent, AgentType>::value, "AgentType must inherit from Agent!");
			std::shared_ptr<AgentType> agent(new AgentType(this, params...));
			m_agents.push_back(agent);
			m_neighbourhoodTree.add(agent);
			return agent;
		}

		/// <summary>
		/// Creates a trigger agent. A trigger is an agent which is not referenced in the neighborhood data structure.
		/// Thus, it can not be perceived by any agent but can do actions in the simulation.
		/// </summary>
		/// <param name="...params">The ...params.</param>
		/// <returns></returns>
		template <typename TriggerType, typename... Parameters>
		std::shared_ptr<TriggerType> createTrigger(Parameters... params)
		{
			m_tasksGroup->wait();
			static_assert(std::is_base_of<Agent, TriggerType>::value, "Trigger must inherit from Agent!");
			std::shared_ptr<AgentType> agent(new AgentType(this, params...));
			m_agents.push_back(agent);
			return agent;
		}

		/// <summary>
		/// Selects the agents in the provided circle.
		/// </summary>
		/// <param name="position">The position.</param>
		/// <param name="radius">The radius.</param>
		/// <returns></returns>
		std::vector<std::shared_ptr<Agent>> selectEntities(const Math::Vector2f & position, float radius)
		{
			return m_neighbourhoodTree.select(position, radius);
		}

		/// <summary>
		/// Selects the agents of the provided type in the provided circle.
		/// </summary>
		/// <param name="position">The position.</param>
		/// <param name="radius">The radius.</param>
		/// <returns></returns>
		template <typename EntityType>
		std::vector<std::shared_ptr<EntityType>> selectEntities(const Math::Vector2f & position, float radius)
		{
			static_assert(std::is_base_of<Agent, EntityType>::value, "Base class of EntityType should be Agent");
			std::vector<std::shared_ptr<Agent>> tmp = selectEntities(position, radius);
			std::vector<std::shared_ptr<EntityType>> result;
			result.reserve(tmp.size());
			for (auto it = tmp.begin(), end = tmp.end(); it != end; ++it)
			{
				std::shared_ptr<EntityType> e = std::dynamic_pointer_cast<EntityType>(*it);
				if (e)
				{
					result.push_back(e);
				}
			}
			return result;
		}

		/// <summary>
		/// Gets the agents.
		/// </summary>
		/// <returns></returns>
		const std::vector<std::shared_ptr<Agent>> & getAgents() const { return m_agents; }

		using message_handler::post;

		/// <summary>
		/// Posts the specified message to the entities lying in the provided circle.
		/// </summary>
		/// <param name="message">The message.</param>
		/// <param name="position">The position.</param>
		/// <param name="radius">The radius.</param>
		template <typename MessageType>
		void post(const MessageType & message, const Math::Vector2f & position, float radius)
		{
			std::vector<std::shared_ptr<Agent>> neighbours = selectEntities(position, radius);
			std::vector<Agent*> agents;
			agents.reserve(neighbours.size());
			std::transform(neighbours.begin(), neighbours.end(), std::back_inserter(agents), [](const std::shared_ptr<Agent> & ptr) { return ptr.get();  });
			post(message, agents);
		}
	};
}