#pragma once

#include <Crowds/Agent.h>
#include <Math/Vectorf.h>

namespace Crowds
{
	/// <summary>
	/// An agent evolving in a 2D environment
	/// </summary>
	/// <seealso cref="Agent" />
	class LocalizedAgent2d : public Agent
	{
	protected:
		Math::Vector2f m_position;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="LocalizedAgent2d"/> class.
		/// </summary>
		/// <param name="simulator">The simulator.</param>
		/// <param name="position">The initial position of the agent.</param>
		LocalizedAgent2d(Simulator * simulator, const Math::Vector2f & position)
			: Agent(simulator), m_position(position)
		{}

		/// <summary>
		/// Gets the agent position.
		/// </summary>
		/// <returns></returns>
		const Math::Vector2f & getPosition() const { return m_position; }
	};
}