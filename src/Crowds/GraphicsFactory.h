#pragma once

#include <GL/glew.h>
#include <Crowds/Agent.h>
#include <SceneGraph/Cylinder.h>
#include <SceneGraph/Translate.h>
#include <SceneGraph/Rotate.h>
#include <SceneGraph/Scale.h>
#include <SceneGraph/DisplayList.h>
#include <HelperGl/Material.h>
#include <HelperGl/HashFunctions.h>
#include <unordered_map>

namespace Crowds
{
	/// <summary>
	/// Class with static methods used to create the graphical representations of the agents
	/// </summary>
	class GraphicsFactory
	{
		/// <summary>
		/// Method returning the geometry of a cylinder. This method facotrizes the graphical represetations.
		/// </summary>
		/// <param name="material">The material of the cylinder.</param>
		/// <returns></returns>
		static SceneGraph::NodeInterface * getCylinder(const HelperGl::Material & material)
		{
			static std::unordered_map<HelperGl::Material, SceneGraph::NodeInterface *> cylinders;
			auto it = cylinders.find(material);
			if (it == cylinders.end())
			{
				SceneGraph::Cylinder * cylinder = new SceneGraph::Cylinder(material);
				SceneGraph::DisplayList * displayList = new SceneGraph::DisplayList(cylinder);
				cylinders[material] = displayList;
				return displayList;
			}
			return it->second;
		}

	public:
		/// <summary>
		/// Link between an agent and its graphic representation.
		/// </summary>
		struct Graphics
		{
			SceneGraph::NodeInterface * m_node;
			SceneGraph::Translate * m_translation;
			SceneGraph::Rotate * m_rotation;
			SceneGraph::Scale * m_scale;
			std::shared_ptr<Agent> m_agent;

			/// <summary>
			/// Updates the position and orientation of the graphic rerpresentation of the agent.
			/// </summary>
			void update()
			{
				m_translation->setTranslation(m_agent->getPosition().pushBack(0.0f));
				m_rotation->setAngle(m_agent->getOrientation());
			}
		};

		/// <summary>
		/// Creates the graphics representation of the provided agent.
		/// </summary>
		/// <param name="agent">The agent.</param>
		/// <param name="material">The material.</param>
		/// <returns> The link between the agent and its graphic representation. </returns>
		static Graphics create(const std::shared_ptr<Agent> & agent, const HelperGl::Material & material)
		{
			SceneGraph::Translate * translate = new SceneGraph::Translate();
			SceneGraph::Rotate * rotate = new SceneGraph::Rotate(0.0f, Math::makeVector(0.0f, 0.0f, 1.0f));
			SceneGraph::Scale * scale = new SceneGraph::Scale(Math::makeVector(agent->getRadius(), agent->getRadius(), agent->getRadius()));
			SceneGraph::NodeInterface * cylinder = getCylinder(material);
			translate->addSon(rotate);
			rotate->addSon(scale);
			scale->addSon(cylinder);
			return { translate, translate, rotate, scale, agent };
		}
	};
}