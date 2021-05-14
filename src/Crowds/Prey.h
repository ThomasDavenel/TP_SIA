#pragma once

#include <Crowds/Boid.h>
#include <Crowds/Water.h>
#include <AI/Tasks/Task.h>

namespace Crowds
{
	class Prey : public Boid
	{
		float previousAngle = 0;
		std::vector<Water *> * pointsEau = new std::vector<Water *>();
		Water * memoireEau = nullptr;
		float soif = Math::Interval<float>(0, 100.0).random();
	public:
		using Boid::Boid;

		Prey(Simulator * simulator, const Math::Vector2f & position, float radius, float mass, float maxSpeed, float maxForce)
			:Boid(simulator, position, radius, mass, maxSpeed, maxForce) 
		{
			createReceiver<WaterMessage>([this](const WaterMessage m) {trouve_eau(m); });
		}

		virtual void killed()
		{
			setPosition(Math::makeVector(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
			setStatus(Status::stopped);
		}

		void trouve_eau(const WaterMessage  &m) {
			pointsEau->push_back(m.m_water);
		}

		virtual void update(double dt) override
		{
			//update soif
			soif = std::clamp<float>(soif - dt, 0, 100);
			//std::cout << soif << " ";
			//Comportement
			setSteeringForce(Math::makeVector(0.0f, 0.0f));
			if ((memoireEau==nullptr) &&  (soif > 10 || pointsEau->empty()) ){
				//std::cout << "JE VAGABONDE" << std::endl;
				std::vector<std::shared_ptr<Prey>> neighbours = perceive<Prey>(10.0f);
				if (mayCollide(neighbours)) {
					addSteeringForce(avoidCollisions(neighbours));
				}
				else {
					std::pair<bool, std::shared_ptr<Prey>> l = plusLourd(neighbours);
					if (l.first) {
						addSteeringForce(arrival(l.second->getPosition(), 1.0f, dt));
						addSteeringForce(separation(neighbours, dt));
					}
					else {
						addSteeringForce(wander(2.0f, 3.0f, previousAngle, dt*5.0));
					}
				}
			}
			else {
				//std::cout << "En pleine recherche d'eau" << std::endl;
				//Memoire de l'eau
				if(memoireEau == nullptr) memoireEau= pointsEau->at(0);
				if(sqrt(pow((getPosition()-memoireEau->getPosition())[0],2) + pow((getPosition() - memoireEau->getPosition())[1], 2))<memoireEau->getRadius()){
					//std::cout << "test" << std::endl;;
					setSteeringForce(stop(dt));
					soif = std::clamp<float>(soif + (dt * 5), 0, 100);
				}
				else {
					std::vector<std::shared_ptr<Prey>> neighbours = perceive<Prey>(10.0f);
					if (mayCollide(neighbours)) {
						addSteeringForce(avoidCollisions(neighbours));
					}
					else {
						addSteeringForce(seek(memoireEau->getPosition(), dt));
					}
				}
				if (soif == 100) {
					memoireEau = nullptr;
				}
			}
			//Affiche pts d'eau
			/*
			int indice = 0;
			for (auto w : pointsEau) {
				indice++;
			}
			std::cout << "nb de points d'eau :" << indice << std::endl;
			*/
			//Supprime pts d'eau
			pointsEau->clear();
			/*~~~~~~*/
			Boid::update(dt);
		}
	private:
		std::pair<bool, std::shared_ptr<Prey>> plusLourd(std::vector<std::shared_ptr<Prey>> neighbours) {
			bool trouve = false;
			std::shared_ptr<Prey> m_lourd= nullptr;
			float masse_tmp = getMass();
			for (auto m : neighbours) {
				if (m->getMass() > masse_tmp) {
					masse_tmp = m->getMass();
					trouve = true;
					m_lourd = m;
				}
			}
			return std::pair<bool, std::shared_ptr<Prey>>(trouve, m_lourd);
		}
	};
}