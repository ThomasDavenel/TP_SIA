#pragma once

#include <map>

namespace stdext
{
	/// <summary>
	/// A map only retaining the k elements having the lesser keys
	/// </summary>
	template <class Key, class Value, class Comparator = std::greater<Key>>
	class kmap
	{
	protected:
		std::map<Key, Value, Comparator> m_map;
		Key m_maxValue;
		size_t m_k;

	public:
		using value_type = typename std::map<Key, Value, Comparator>::value_type;
		using iterator = typename std::map<Key, Value, Comparator>::iterator;
		using const_iterator = typename std::map<Key, Value, Comparator>::const_iterator;
		using reverse_iterator = typename std::map<Key, Value, Comparator>::reverse_iterator;
		using const_reverse_iterator = typename std::map<Key, Value, Comparator>::const_reverse_iterator;

		/// <summary>
		/// Initializes a new instance of the <see cref="kmap"/> class.
		/// </summary>
		/// <param name="k">The maximum number of elements in the map.</param>
		kmap(size_t k)
			: m_k(k)
		{
			assert(k > 0);
		}

		/// <summary>
		/// Inserts the specified value in the map. This value can be retained or discarded if it is not among the ones having the k lesser keys.
		/// </summary>
		/// <param name="value">The value to insert.</param>
		void insert(const value_type & value)
		{
			Comparator compare;
			if (has_max()) 
			{ 
				//if (compare(value.first, m_maxValue))
				if (compare(m_maxValue, value.first))
				{
					//std::cout << "Before: " << m_map.size();
					m_map.insert(value);
					//std::cout << "After: " << m_map.size();
					//m_map.erase(m_map.rbegin()->first);
					m_map.erase(m_map.begin());
					//std::cout << "Finally: " << m_map.size() << std::endl;
					//m_maxValue = (m_map.rbegin()->first);
					m_maxValue = m_map.begin()->first;
				}
			}
			else
			{
				m_map.insert(value);
				if (has_max()) // If there is a max after insertion, we update it
				{
					//m_maxValue = (m_map.rbegin()->first);
					m_maxValue = m_map.begin()->first;
				}
			}
		}

		/// <summary>
		/// Determines whether this instance has a maximum value.
		/// </summary>
		/// <returns>
		///   <c>true</c> if this instance has maximum value; otherwise, <c>false</c>.
		/// </returns>
		bool has_max() const { return m_map.size() == m_k; }

		/// <summary>
		/// Gets the maximum value if available (see has_max). Please note that a maximum the maximum value is available as soon as the 
		/// kmap reaches its maximum number of elements. In such a case, all values greater or equal to max() will be discarded.
		/// </summary>
		/// <returns></returns>
		const Key & max() const
		{
			assert(has_max());
			return m_maxValue;
		}

		iterator begin() { return m_map.begin(); }

		iterator end() { return m_map.end(); }

		const_iterator begin() const { return m_map.begin(); }

		const_iterator end() const { return m_map.end(); }

		reverse_iterator rbegin() { return m_map.rbegin(); }

		reverse_iterator rend() { return m_map.rend(); }

		const_reverse_iterator rbegin() const { return m_map.rbegin(); }

		const_reverse_iterator rend() const { return m_map.rend(); }

		iterator erase(const_iterator pos) { return m_map.erase(pos); }

		iterator erase(const_iterator first, const_iterator last) { m_map.erase(first, last); }
	};
}