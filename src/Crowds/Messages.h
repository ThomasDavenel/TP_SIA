#pragma once

#include <Crowds/Agent.h>

namespace Crowds
{
	class Prey;
	class Predator;
	class Water;

	/// <summary>
	/// Message sent when a collision between entities is detected
	/// </summary>
	struct CollisionMessage 
	{
		Agent * m_emitter;
		Agent * m_collisioner;
	};

	/// <summary>
	/// Message sent when an agent belongs to a group of agents (i.e. following behavior)
	/// </summary>
	struct BelongsToGroupMessage 
	{
		Agent * m_emitter;
	};

	/// <summary>
	/// Message sent when a prey panics
	/// </summary>
	struct PanicMessage 
	{
		Prey * m_emitter;
		std::shared_ptr<Predator> m_predator;
	};

	/// <summary>
	/// Message sent when a prey is killed.
	/// </summary>
	struct PreyKilledMessage
	{
		Agent * m_emitter;
	};

	/// <summary>
	/// Message sent to the agent when they are near some water.
	/// </summary>
	struct WaterMessage
	{
		Water * m_water;
	};
}