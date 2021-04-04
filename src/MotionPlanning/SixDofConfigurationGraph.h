#pragma once

#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/VPTree.h>
#include <unordered_set>
#include <stdext/disjoint_set.h>

namespace MotionPlanning
{
	/// <summary>
	/// A configuration graph that can be used as the underlying structure of roadmaps.
	/// Nodes are tagged with a configuration and a <see cref="SixDofConfigurationGraph::ConfigurationSpaceQualifier"> configuration space qualifier </see>.
	/// Edges are tagged with the distance between configurations and a <see cref="SixDofConfigurationGraph::ConfigurationSpaceQualifier"> configuration space qualifier </see>.
	/// </summary>
	class SixDofConfigurationGraph
	{
	public:
		/// <summary>
		/// An enum class that enables to qualify 
		/// </summary>
		enum class ConfigurationSpaceQualifier { cFree, cObstable, unkown, both };

		/// <summary>
		/// A node of the graph.
		/// </summary>
		class Node
		{
			friend class SixDofConfigurationGraph;

		public:
			struct Transition
			{
				/// <summary>
				/// The extremity of the transition.
				/// </summary>
				Node * m_extremity;
				/// <summary>
				/// The distance between the source and the destination configuration.
				/// </summary>
				float  m_distance;
				/// <summary>
				/// The configuration space qualifier associated with the path described by the transition.
				/// </summary>
				ConfigurationSpaceQualifier m_spaceQualifier;
			};

		protected:
			SixDofPlannerBase::Configuration m_configuration;
			std::vector<Transition> m_outgoingTransitions;
			ConfigurationSpaceQualifier m_spaceQualifier;

			/// <summary>
			/// Adds a transition from this node to another.
			/// </summary>
			/// <param name="node">The node.</param>
			void addTransition(Node * node, ConfigurationSpaceQualifier spaceQualifier)
			{
				m_outgoingTransitions.push_back(Transition{ node, SixDofPlannerBase::configurationDistance(m_configuration, node->m_configuration), spaceQualifier });
			}

			/// <summary>
			/// Initializes a new instance of the <see cref="Node"/> class.
			/// </summary>
			/// <param name="configuration">The configuration.</param>
			/// <param name="space">The space qualifier.</param>
			Node(const SixDofPlannerBase::Configuration & configuration, ConfigurationSpaceQualifier space)
				: m_configuration(configuration), m_spaceQualifier(space)
			{}

		public:
			/// <summary>
			/// Gets the configuration.
			/// </summary>
			/// <returns></returns>
			const SixDofPlannerBase::Configuration & getConfiguration() const { return m_configuration; }

			/// <summary>
			/// Gets the configuration space qualifier.
			/// </summary>
			/// <returns></returns>
			ConfigurationSpaceQualifier getConfigurationSpace() const { return m_spaceQualifier; }
			
			/// <summary>
			/// Gets the outgoing transitions.
			/// </summary>
			/// <returns></returns>
			const std::vector<Transition> & getOutgoingTransitions() const { return m_outgoingTransitions; }
		};

		/// <summary>
		/// The tree used for neighbouhood search.
		/// </summary>
		VPTree<Node*, SixDofPlannerBase::Configuration> * m_tree;

		/// <summary>
		/// The nodes contained in the graph
		/// </summary>
		std::vector<Node*> m_nodes;
		size_t m_nbEdges;

		/// <summary>
		/// The disjoint set used to track the connected components of the graph
		/// </summary>
		stdext::disjoint_set<Node*> m_connectedComponents;

	public:
		using ConnectedComponentnId = stdext::disjoint_set<Node*>::set_id;

		/// <summary>
		/// Initializes a new instance of the <see cref="SixDofConfigurationGraph"/> class.
		/// </summary>
		SixDofConfigurationGraph()
			: m_nbEdges(0)
		{
			auto distanceNode = [](Node * n1, Node * n2) { return SixDofPlannerBase::configurationDistance(n1->getConfiguration(), n2->getConfiguration()); };
			auto distanceNodeConfiguration = [](Node * n1, const SixDofPlannerBase::Configuration & configuration)
			{
				return SixDofPlannerBase::configurationDistance(n1->getConfiguration(), configuration);
			};
			m_tree = new VPTree<Node*, SixDofPlannerBase::Configuration>(distanceNode, distanceNodeConfiguration);
		}

		~SixDofConfigurationGraph()
		{
			for (auto it = m_nodes.begin(), end = m_nodes.end(); it != end; ++it)
			{
				delete (*it);
			}
		}

		SixDofConfigurationGraph(const SixDofConfigurationGraph &) = delete;
		SixDofConfigurationGraph & operator=(const SixDofConfigurationGraph &) = delete;

		/// <summary>
		/// Adds a new node in the graph recording the provided configuration.
		/// </summary>
		/// <param name="configuration">The configuration.</param>
		/// <returns></returns>
		Node * add(const SixDofPlannerBase::Configuration & configuration, ConfigurationSpaceQualifier space = ConfigurationSpaceQualifier::cFree)
		{
			Node * node = new Node(configuration, space);
			m_nodes.push_back(node);
			m_tree->add(node);
			m_connectedComponents.insert(node);
			return node;
		}

		/// <summary>
		/// Adds an edge in the graph.
		/// </summary>
		/// <param name="node1">The source.</param>
		/// <param name="node2">The destination.</param>
		void addBidirectionalEdge(Node * node1, Node * node2, ConfigurationSpaceQualifier space = ConfigurationSpaceQualifier::cFree)
		{
			node1->addTransition(node2, space);
			node2->addTransition(node1, space);
			m_connectedComponents.merge(node1, node2); // Now the connected components containing node1 and node2 are connected
			++m_nbEdges;
		}

		/// <summary>
		/// Returns the number of nodes
		/// </summary>
		/// <returns></returns>
		size_t nodesSize() const { return m_nodes.size(); }

		/// <summary>
		/// Returns the number of edges in the graph.
		/// </summary>
		/// <returns></returns>
		size_t edgesSize() const { return m_nbEdges; }

		/// <summary>
		/// Returns the number of connected components in the graph.
		/// </summary>
		/// <returns></returns>
		size_t connectedComponentsSize() const { return m_connectedComponents.size(); }

		/// <summary>
		/// Gets the identifier of the connected component containing the provided node.
		/// </summary>
		/// <param name="node">The node.</param>
		/// <returns></returns>
		ConnectedComponentnId getConnectedComponent(Node * node) const
		{
			return m_connectedComponents.get_set_identifier(node);
		}

		/// <summary>
		/// Returns the k nearest neighbours of a configuration.
		/// </summary>
		/// <param name="configuration">The configuration.</param>
		/// <param name="k">The k.</param>
		/// <returns> The k nearest neighbours of the provided configuration.</returns>
		std::vector<Node*> kNearestNeighbours(const SixDofPlannerBase::Configuration & configuration, size_t k) const
		{
			return m_tree->kNearestNeighbour(configuration, k);
		}

		/// <summary>
		/// Search for the nearest neighbour of a configuration.
		/// </summary>
		/// <param name="configuration">The configuration.</param>
		/// <returns> The nearest neighbour </returns>
		Node * nearestNeighbour(const SixDofPlannerBase::Configuration & configuration) const
		{
			return m_tree->nearestNeighbour(configuration);
		}

		/// <summary>
		/// Selects all nodes lying in the ball of center "configuration" and of radius "radius".
		/// </summary>
		/// <param name="configuration">The center of the ball.</param>
		/// <param name="radius">The radius of the ball.</param>
		/// <returns> The nodes lying in the ball.</returns>
		std::vector<Node*> select(const SixDofPlannerBase::Configuration & configuration, float radius) const
		{
			return m_tree->select(configuration, radius);
		}
	};
}
