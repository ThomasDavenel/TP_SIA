#pragma once

#include <Crowds/Boid.h>

namespace Crowds
{
	class Predator : public Boid
	{
	private:
		float previousAngle = 0;
		std::shared_ptr<AI::Tasks::Task> task_wander;
		float dTps = 0;
	public:
		using Boid::Boid;

		Predator(Simulator * simulator, const Math::Vector2f & position, float radius, float mass, float maxSpeed, float maxForce)
			:Boid(simulator, position, radius, mass, maxSpeed, maxForce)
		{
			task_wander = AI::Tasks::makeTask([this]() { return fonction_wander(); });
		}

		AI::Tasks::Task::Status fonction_wander() {
			std::vector<std::shared_ptr<Predator>> neighbours = perceive<Predator>(10.0f);
			if (mayCollide(neighbours)) {
				addSteeringForce(avoidCollisions(neighbours));
			}
			else {
					addSteeringForce(wander(2.0f, 3.0f, previousAngle, dTps *5.0));
			}
			return AI::Tasks::Task::Status::running;
		}

		virtual void update(double dt) override
		{
			dTps = dt;
			//FAIRE UNE TACHE MAIS AVEC DES OPERATEUR ET DES PR2CONDITION
			//CREER UNE TACHE DE PRECONDITION
			//PARALLELISME DE TACHE ==> OPERATOR AUSSI

			std::vector<std::shared_ptr<Prey>> l_prey = perceive<Prey>(15.0f);
			if (l_prey.empty()) {
				task_wander->execute();
			}
			else {

			}
			Boid::update(dt);
		}
	};
}