#pragma once
#pragma once
#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/SixDofConfigurationGraph.h>
#include <Math/Vectorf.h>
#include <Math/UniformRandom.h>
#include <Math/Interpolation.h>
#include <Math/Quaternion.h>
#include <MotionPlanning/CollisionManager.h>
#include <Math/Constant.h>
#include <vector>
#include <queue>

namespace MotionPlanning
{
	/// <summary>
	/// Base class for six dof planners
	/// </summary>
	class PRM : public SixDofPlannerBase
	{
		SixDofConfigurationGraph graphe;
		size_t k_voisin = 10;
	public:
		PRM(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::initializer_list<std::pair<float, float>>& intervals = { defaultPositionInterval(), defaultPositionInterval(), defaultPositionInterval(), defaultAngleInterval(), defaultAngleInterval(), defaultAngleInterval() }) :
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}

		PRM(MotionPlanning::CollisionManager* collisionManager, MotionPlanning::CollisionManager::DynamicCollisionObject object, const std::vector<std::pair<float, float>>& intervals) :
			SixDofPlannerBase(collisionManager, object, intervals)
		{
		}
		void 	grow(size_t nbNodes, size_t k, float dq, size_t maxSamples = std::numeric_limits< size_t >::max()) {
			size_t current_nbNodes = 0;
			size_t current_sample = 0;
			while (current_nbNodes < nbNodes && current_sample < maxSamples) {
				//On cree une nouvelle config
				Configuration rnd_Config = randomConfiguration();
				current_sample++;
				//Si elle est sur le free space on l'ajoute au graphe
				if (!doCollide(rnd_Config)) {
					MotionPlanning::SixDofConfigurationGraph::Node * newNode = graphe.add(rnd_Config);
					current_nbNodes++;
					//On la relie a ses k voisin si c'est possible 
					for (MotionPlanning::SixDofConfigurationGraph::Node * c : graphe.kNearestNeighbours(rnd_Config, k)) {
						bool sameComposante = true;
						if (sameComposante || graphe.getConnectedComponent(c) != graphe.getConnectedComponent(newNode)) {
							if (!doCollide(rnd_Config, c->getConfiguration(), dq)) {
								graphe.addBidirectionalEdge(c, newNode);
								sameComposante = false;
							}

						}
					}
				}
			}
		}
		virtual bool 	plan(const Configuration &start, const Configuration &target, float radius, float dq, std::vector< Configuration > &result) override {
			size_t k = 20;
			//On cree le graphe
			//grow(50000, k, dq, 500000);
			//On relie le start et le target au graphe
			MotionPlanning::SixDofConfigurationGraph::Node * nodeStart = graphe.add(start);
			MotionPlanning::SixDofConfigurationGraph::Node * nodeTarget = graphe.add(target);
			for (auto c : graphe.kNearestNeighbours(start, k)) {
				if (!doCollide(start, c->getConfiguration(), dq)) {
					graphe.addBidirectionalEdge(c, nodeStart);
				}
			}
			for (auto c : graphe.kNearestNeighbours(target, k)) {
				if (!doCollide(target, c->getConfiguration(), dq)) {
					graphe.addBidirectionalEdge(c, nodeTarget);
				}
			}
			return aEtoile(nodeStart, nodeTarget, dq, k, result);
		}
	private:
		bool aEtoile(MotionPlanning::SixDofConfigurationGraph::Node *  start, MotionPlanning::SixDofConfigurationGraph::Node * target, float dq, size_t k, std::vector< Configuration > &result) {
			while (true) {
				//Si target et start sont dans la même composante connexe alors une solution existe, sinon non, on refait grossir le graphe
			//On utilise A* pour trouver le chemin le plus court entre start et target
				if (graphe.getConnectedComponent(start) == graphe.getConnectedComponent(target)) {
					std::cout << "on commence la recherche avec A*" << std::endl;
					std::map< MotionPlanning::SixDofConfigurationGraph::Node *, float> cost;
					std::map< MotionPlanning::SixDofConfigurationGraph::Node *, MotionPlanning::SixDofConfigurationGraph::Node *> pred;
					std::queue <std::pair<float, MotionPlanning::SixDofConfigurationGraph::Node *>> q;
					std::pair<float, MotionPlanning::SixDofConfigurationGraph::Node *> pairDebut(configurationDistance(start->getConfiguration(), target->getConfiguration()), start);
					q.push(pairDebut);
					cost[start] = 0.f;
					while (!q.empty()) {
						std::pair<float, MotionPlanning::SixDofConfigurationGraph::Node *> c = q.front();
						if (c.second == target) {
							computeRes(start, target, pred, result);
							return true;
						}
						else {
							//std::cout << "debut" << std::endl;
							for (auto t : c.second->getOutgoingTransitions()) {
								//std::cout <<"c.second : "<< cost[c.second] << std::endl;
								MotionPlanning::SixDofConfigurationGraph::Node * s = t.m_extremity;
								if (cost.find(s) == cost.end()) {
									cost[s] = 999999;
									//std::cout << "test cost :"<<cost[s]<<std::endl;
								}
								if (costF(s, c.second) + cost[c.second] < cost[s]) {
									std::pair<float, MotionPlanning::SixDofConfigurationGraph::Node *> newPaire(costF(s, c.second) + cost[c.second] + costF(s, target), s);
									q.push(newPaire);
									cost[s] = costF(s, c.second) + cost[c.second];
									//std::cout << costF(s, c.second) << std::endl;
									pred[s] = c.second;
								}
							}
						}
						//std::cout << "fin" << std::endl;
						q.pop();
					}
					std::cout << "ERREUR :pas de chemin trouver " << std::endl;
					return false;
				}
				else {
					std::cout << "le start et le target ne font pas partie de la meme composante connexe --> on refais grossir le graphe" << std::endl;
					grow(10, k, dq);
				}
			}
		}
		float costF(MotionPlanning::SixDofConfigurationGraph::Node * a, MotionPlanning::SixDofConfigurationGraph::Node *b) {
			if(a!=b) return configurationDistance(a->getConfiguration(), b->getConfiguration());
			else return 0;
		}
		void computeRes(MotionPlanning::SixDofConfigurationGraph::Node *  start, MotionPlanning::SixDofConfigurationGraph::Node * target, std::map< MotionPlanning::SixDofConfigurationGraph::Node * , MotionPlanning::SixDofConfigurationGraph::Node *> pred, std::vector< Configuration > &result) {
			MotionPlanning::SixDofConfigurationGraph::Node * currentNode = target;
			result.push_back(target->getConfiguration());
			while (pred[currentNode]!=start) {
				result.push_back(pred[currentNode]->getConfiguration());
				currentNode = pred[currentNode];
			}
			result.push_back(start->getConfiguration());
			std::reverse(result.begin(), result.end());
		}
	};
}