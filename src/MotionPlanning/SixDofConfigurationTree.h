#pragma once

#include <MotionPlanning/SixDofPlannerBase.h>
#include <MotionPlanning/VPTree.h>
#include <unordered_set>

namespace MotionPlanning
{
	/// <summary>
	/// A tree of Six DOF configurations with nearest neighbourhood search and ball selection capabilities.
	/// </summary>
	class SixDofConfigurationTree
	{
	public:
		/// <summary>
		/// A node of the tree.
		/// </summary>
		class Node
		{
		private:
			friend class SixDofConfigurationTree;
			
			SixDofPlannerBase::Configuration m_configuration;
			std::vector<Node *> m_sons;
			Node * m_father;

			/// <summary>
			/// Initializes a new instance of the <see cref="Node"/> class. Only the class <see cref="MotionPlanning::SixDofConfigurationTree"> MotionPlanning::SixDofConfigurationTree </see> can call this constructor.
			/// </summary>
			/// <param name="configuration">The configuration.</param>
			Node(const SixDofPlannerBase::Configuration & configuration = SixDofPlannerBase::Configuration())
				: m_configuration(configuration), m_father(nullptr)
			{}

			/// <summary>
			/// Adds a new son to this node.
			/// </summary>
			/// <param name="son">The son.</param>
			void addSon(Node * son)
			{
				m_sons.push_back(son);
				son->m_father = this;
			}

			/// <summary>
			/// Reverts the branch such as the node with the nullptr parameter value becomes the new root of the tree/
			/// </summary>
			/// <param name="newFather">The new father.</param>
			void markAsRoot(Node * newFather = nullptr)
			{
				if (m_father != nullptr)
				{
					m_father->markAsRoot(this);
				}
				if (newFather != nullptr)
				{
					newFather->addSon(this);
					auto it = std::find(m_sons.begin(), m_sons.end(), newFather);
					assert(it != m_sons.end());
					(*it) = m_sons.back();
					m_sons.pop_back();
				}
				else
				{
					m_father = nullptr;
				}
			}

		public:
			/// <summary>
			/// Gets the father of this node.
			/// </summary>
			/// <returns></returns>
			Node * getFather() const { return m_father; }

			/// <summary>
			/// Gets the sons of this node.
			/// </summary>
			/// <returns></returns>
			const std::vector<Node*> & getSons() const { return m_sons; }

			/// <summary>
			/// Gets the configuration associated with this node.
			/// </summary>
			/// <returns></returns>
			const SixDofPlannerBase::Configuration & getConfiguration() const
			{
				return m_configuration;
			}

			/// <summary>
			/// Finalizes an instance of the <see cref="Node"/> class.
			/// </summary>
			~Node()
			{
				for (auto it = m_sons.begin(), end = m_sons.end(); it != end; ++it)
				{
					delete (*it);
				}
			}
		};

	private:
		VPTree<Node*, SixDofPlannerBase::Configuration> * m_tree;
		Node * m_root;
		std::unordered_set<Node*> m_leaves;
		
		/// <summary>
		/// Test whether the node belongs to this tree. Used for debug purpose only. 
		/// </summary>
		/// <param name="node">The node.</param>
		/// <returns></returns>
		bool doesBelong(Node * node) const
		{
			while (node->getFather() != nullptr)
			{
				node = node->getFather();
			}
			return node == m_root;
		}

		/// <summary>
		/// Collects all the nodes belonging to the tree.
		/// </summary>
		/// <param name="node">The node.</param>
		/// <param name="result">The result.</param>
		void collectNodes(Node * node, std::vector<Node*> & result)
		{
			result.push_back(node);
			for(auto it=node->getSons().begin(), end=node->getSons().end() ; it!=end ; ++it)
			{
				collectNodes(*it, result);
			}
		}
		
	public:
		/// <summary>
		/// Creates an empty tree
		/// </summary>
		SixDofConfigurationTree()
			: 
			//m_tree([this](Node * node, Node * node2) -> float { return SixDofPlannerBase::configurationDistance(node->getConfiguration(), node2->getConfiguration()); },
			//	[this](Node * node, const SixDofPlannerBase::Configuration & config) -> float {return SixDofPlannerBase::configurationDistance(node->getConfiguration(), config); })
			//, 
			m_root(nullptr)
		{
			m_tree = new VPTree<Node*, SixDofPlannerBase::Configuration>
				([this](Node * node, Node * node2) -> float { return SixDofPlannerBase::configurationDistance(node->getConfiguration(), node2->getConfiguration()); },
				 [this](Node * node, const SixDofPlannerBase::Configuration & config) -> float {return SixDofPlannerBase::configurationDistance(node->getConfiguration(), config); });

		}

		/// <summary>
		/// Finalizes an instance of the <see cref="SixDofConfigurationTree"/> class.
		/// </summary>
		~SixDofConfigurationTree()
		{
			if (m_root != nullptr) { delete m_root; }
			if (m_tree != nullptr) { delete m_tree; }
		}

		/// <summary>
		/// Move constructor.
		/// </summary>
		/// <param name="other">The other tree.</param>
		SixDofConfigurationTree(SixDofConfigurationTree && other)
			: m_tree(other.m_tree), m_root(other.m_root), m_leaves(std::move(other.m_leaves))
		{
			other.m_root = nullptr;
			other.m_tree = nullptr;
		}

		/// <summary>
		/// Move assignment operator.
		/// </summary>
		/// <param name="other">The other tree.</param>
		/// <returns></returns>
		SixDofConfigurationTree & operator = (SixDofConfigurationTree && other)
		{
			std::swap(m_tree, other.m_tree);
			std::swap(m_root, other.m_root);
			std::swap(m_leaves, other.m_leaves);
			return (*this);
		}

		SixDofConfigurationTree(const SixDofConfigurationTree &) = delete;
		SixDofConfigurationTree & operator = (const SixDofConfigurationTree &) = delete;

		/// <summary>
		/// Creates a node in the tree.
		/// </summary>
		/// <param name="configuration">The configuration associated with the node.</param>
		/// <param name="father">The father of the created node. If the father is nullptr, the created node becomes the root of the tree.</param>
		/// <returns> The newly created node. </returns>
		Node * createNode(const SixDofPlannerBase::Configuration & configuration, Node * father = nullptr)
		{
			assert(father != nullptr || m_root == nullptr);
			assert(father==nullptr || doesBelong(father));
			Node * created = new Node(configuration);
			if(father==nullptr)
			{
				m_root = created;
			}
			else
			{
				father->addSon(created);
				m_leaves.erase(father);
			}
			m_leaves.insert(created);
			m_tree->add(created);
			return created;
		}

		/// <summary>
		/// Gets the leaves of the tree.
		/// </summary>
		/// <returns></returns>
		const std::unordered_set<Node*> & getLeaves() const { return m_leaves; }
		
		/// <summary>
		/// Finds the node which minimizes the distance with the provided configuration.
		/// </summary>
		/// <param name="configuration">The configuration.</param>
		/// <returns> The node which minimizes the distance with the provided configuration </returns>
		Node * nearest(const SixDofPlannerBase::Configuration & configuration) const
		{
			if (m_root == nullptr) { return nullptr; }
			return m_tree->nearestNeighbour(configuration);
		}

		/// <summary>
		/// Selects all the nodes lying in the ball centered at configuration with radius radius.
		/// </summary>
		/// <param name="configuration">The configuration which is the center of the ball.</param>
		/// <param name="radius">The radius of the ball.</param>
		/// <returns></returns>
		std::vector<Node*> select(const SixDofPlannerBase::Configuration & configuration, float radius) const
		{
			return m_tree->select(configuration, radius);
		}

		/// <summary>
		/// Sets the new root.
		/// </summary>
		/// <param name="node">The node.</param>
		void setNewRoot(Node * node)
		{
			assert(doesBelong(node));
			node->markAsRoot();
			m_root = node;
		}

		/// <summary>
		/// Merges this tree and the provided one such as the provided tree becomes a son of the provided node.
		/// The provided node must belong to this tree. Please note that the provided tree if set to empty afterwards.
		/// </summary>
		/// <param name="attachmentTarget">The node to which the other tree must be attached.</param>
		/// <param name="toAttach">The tree to attach.</param>
		void merge(Node * attachmentTarget, SixDofConfigurationTree & toAttach)
		{
			std::vector<Node*> nodes;
			toAttach.collectNodes(toAttach.m_root, nodes);
			attachmentTarget->addSon(toAttach.m_root);
			for(auto it=nodes.begin(), end=nodes.end() ; it!=end ; ++it)
			{
				m_tree->add(*it);
			}
			toAttach.m_root = nullptr;
			toAttach.m_tree->clear();
		}

		/// <summary>
		/// Determines whether this tree is empty.
		/// </summary>
		/// <returns>
		///   <c>true</c> if this tree is empty; otherwise, <c>false</c>.
		/// </returns>
		bool isEmpty() const { return m_root == nullptr; }

		/// <summary>
		/// Clears this tree.
		/// </summary>
		void clear()
		{
			if (m_root) { delete m_root; }
			m_root = nullptr;
			m_tree->clear();
		}

		/// <summary>
		/// Returns the number of elements in the tree.
		/// </summary>
		/// <returns></returns>
		size_t size()
		{
			return m_tree->size();
		}
	};
}