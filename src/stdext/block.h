#pragma once

#include <array>

namespace stdext
{
	/// <summary>
	/// A class having the same properties as std::array but also bahaving as std::vector (except for the resize aspect)
	/// </summary>
	template <typename Type, size_t blockSize>
	class block 
	{
		std::array<Type, blockSize> m_data;
		size_t m_size;

	public:
		using value_type = typename std::array<Type, blockSize>::value_type;
		using size_type = typename std::array<Type, blockSize>::size_type;
		using difference_type = typename std::array<Type, blockSize>::difference_type;
		using reference = typename std::array<Type, blockSize>::reference;
		using const_reference = typename std::array<Type, blockSize>::const_reference;
		using pointer = typename std::array<Type, blockSize>::pointer;
		using const_pointer = typename std::array<Type, blockSize>::const_pointer;
		using iterator = typename std::array<Type, blockSize>::iterator;
		using const_iterator = typename std::array<Type, blockSize>::const_iterator;
		using reverse_iterator = typename std::array<Type, blockSize>::reverse_iterator;
		using const_reverse_iterator = typename std::array<Type, blockSize>::const_reverse_iterator;

		/// <summary>
		/// Creation of a new block.
		/// </summary>
		block()
			: m_size(0)
		{}

		/// <summary>
		/// Pushes the value at the end of the vector.
		/// </summary>
		/// <param name="value">The value.</param>
		void push_back(const_reference value)
		{
			assert(m_size < blockSize);
			m_data[m_size] = value;
			++m_size;
		}

		/// <summary>
		/// Pops the value at the end of the vector.
		/// </summary>
		void pop_back()
		{
			assert(m_size > 0);
			--m_size;
		}

		/// <summary>
		/// access specified element with bounds checking 
		/// </summary>
		/// <param name="index">The index.</param>
		/// <returns></returns>
		reference at(size_t index)
		{
			return m_data.at(index);
		}

		/// <summary>
		/// access specified element with bounds checking 
		/// </summary>
		/// <param name="index">The index.</param>
		/// <returns></returns>
		const_reference at(size_t index) const
		{
			return m_data.at(index);
		}

		/// <summary>
		/// access the first element 
		/// </summary>
		/// <returns></returns>
		reference front()
		{
			return m_data.front();
		}

		/// <summary>
		/// access the first element 
		/// </summary>
		/// <returns></returns>
		const_reference front() const
		{
			return m_data.front();
		}

		/// <summary>
		/// access the last element 
		/// </summary>
		/// <returns></returns>
		reference back()
		{
			return m_data.back();
		}

		/// <summary>
		/// access the last element 
		/// </summary>
		/// <returns></returns>
		const_reference back() const
		{
			return m_data.back();
		}

		/// <summary>
		/// Access to a value with its index.
		/// </summary>
		/// <param name="index">The index.</param>
		/// <returns></returns>
		reference operator[] (size_t index)
		{
			return m_data[index];
		}

		/// <summary>
		/// Access to a value with its index.
		/// </summary>
		/// <param name="index">The index.</param>
		/// <returns></returns>
		const_reference operator[] (size_t index) const
		{
			assert(index < m_size);
			return m_data[index];
		}

		/// <summary>
		/// Returns a raw pointer to the data in the block
		/// </summary>
		/// <returns></returns>
		const_pointer data() const { return m_data.data(); }

		/// <summary>
		/// Returns a raw pointer to the data in the block
		/// </summary>
		pointer data() { return m_data.data(); }

		iterator begin() { return m_data.begin(); }
		const_iterator begin() const { return m_data.begin(); }
		const_iterator cbegin() { return m_data.cbegin(); }
		iterator end() { return m_data.end(); }
		const_iterator end() const { return m_data.end(); }
		const_iterator cend() { return m_data.cend(); }

		iterator rbegin() { return m_data.rbegin(); }
		const_iterator rbegin() const { return m_data.rbegin(); }
		const_iterator crbegin() { return m_data.crbegin(); }
		iterator rend() { return m_data.rend(); }
		const_iterator rend() const { return m_data.rend(); }
		const_iterator crend() { return m_data.crend(); }

		bool empty() const { return m_size == 0; }

		/// <summary>
		/// Returns the size of the block
		/// </summary>
		/// <returns></returns>
		size_t size() const { return m_size; }

		size_t max_size() const { return blockSize; }

		void fill(const_reference value) { m_data.fill(value); }

		void swap(const block & other) { m_data.swap(other.m_data); }
	};
}