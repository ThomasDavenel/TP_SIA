#pragma once

#include <Crowds/Messages.h>
#include <Crowds/Agent.h>

namespace Crowds
{
	class Water : public Agent
	{
	public:
		Water(Simulator * simulator, const Math::Vector2f & position, float radius)
			: Agent(simulator, position, radius)
		{}

		virtual void update(double dt) override
		{
			WaterMessage w;
			w.m_water = this;
			this->post(w,this->getRadius()+10);
		}
	};
}