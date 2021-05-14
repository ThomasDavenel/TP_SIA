#pragma once

#include <unordered_map>
#include <algorithm>

namespace stdext
{
	/// <summary>
	/// A disjoint set data structure.
	/// </summary>
	template <typename Key, typename Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
	class disjoint_set
	{
		/// <summary>
		/// Internal tree class
		/// </summary>
		class tree_node
		{
			mutable tree_node * m_father;
			mutable size_t m_rank;
			Key m_element;

		public:
			/// <summary>
			/// Initializes a new instance of the <see cref="disjoint_set"/> class.
			/// </summary>
			/// <param name="value">The value contained by the node.</param>
			tree_node(const Key & value)
				: m_father(nullptr), m_rank(0), m_element(value)
			{}

			/// <summary>
			/// Gets the rank of the node (distance to leaf...)
			/// </summary>
			/// <returns></returns>
			size_t getRank() const { return m_rank; }

			/// <summary>
			/// Gets the root of the tree containing the node.
			/// </summary>
			/// <returns></returns>
			tree_node * getRoot() const
			{
				tree_node * current = (tree_node*)this;
				while (current->m_father != nullptr)
				{
					current = current->m_father;
				}
				if (current != this) { m_father = current; m_rank = 0; }
				return current;
			}

			/// <summary>
			/// Attaches this tree as a sub tree of the provided tree.
			/// </summary>
			/// <param name="newRoot">The new root.</param>
			void attachTo(tree_node * newRoot)
			{
				m_father = newRoot;
				m_father->m_rank = std::max(m_father->m_rank, m_rank+1);
			}
		};

		std::unordered_map<Key, tree_node*, Hash, KeyEqual> m_map;
		size_t m_nbSets;

	public:		
		/// <summary>
		/// Type used to identity a tree.
		/// </summary>
		using set_id = const void *;

		/// <summary>
		/// Initializes a new instance of the <see cref="disjoint_set"/> class.
		/// </summary>
		disjoint_set() : m_nbSets(0) {}

		/// <summary>
		/// Inserts the specified value in its own set.
		/// </summary>
		/// <param name="value">The value to insert.</param>
		/// <returns></returns>
		bool insert(const Key & value)
		{
			auto it = m_map.find(value);
			if (it != m_map.end()) { return false; }
			tree_node * node = new tree_node(value);
			m_map.insert({ value, node });
			m_nbSets++;
			return true;
		}

		/// <summary>
		/// Merges the sets of the two provided values. If the values already belong to the same set, nothing happens, otherwise the sets are merged.
		/// </summary>
		/// <param name="value1">The value1.</param>
		/// <param name="value2">The value2.</param>
		/// <returns></returns>
		bool merge(const Key & value1, const Key & value2)
		{
			tree_node * root1 = m_map.find(value1)->second->getRoot();
			tree_node * root2 = m_map.find(value2)->second->getRoot();
			if (root1 != root2)
			{
				if (root1->getRank() > root2->getRank())
				{
					root2->attachTo(root1);
				}
				else
				{
					root1->attachTo(root2);
				}
				m_nbSets--;
				return true;
			}
			return false;
		}

		/// <summary>
		/// Gets the identifier of the set containing the provided value.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <returns></returns>
		set_id get_set_identifier(const Key & value) const
		{
			return m_map.find(value)->second->getRoot();
		}

		/// <summary>
		/// Returns the number of disjoint sets.
		/// </summary>
		/// <returns></returns>
		size_t size() const { return m_nbSets; }

		/// <summary>
		/// Computes the sets contained in this disjoint set.
		/// </summary>
		/// <returns></returns>
		std::vector<std::vector<Key>> compute_sets() const
		{
			std::unordered_map<set_id, std::vector<Key>> result;
			for (auto it = m_map.begin(), end = m_map.end(); it != end; ++it)
			{
				result[it->second->getRoot()].push_back(it->first);
			}
			std::vector<Key> toReturn;
			toReturn.reserve(result.size());
			for (auto it = result.begin(), end = result.end(); it != end; ++it)
			{
				toReturn.emplace_back(std::move(it->second));
			}
		}
	};
}