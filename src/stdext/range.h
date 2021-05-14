#pragma once

#include <cassert>
#include <type_traits>
#include <iterator>

namespace stdext
{
	/// <summary>
	/// A range of iterators. It is part of the C++ 20 specs (min a more complete version)
	/// </summary>
	template <typename iterator_template>
	class range
	{
	public:
		using iterator = iterator_template;

		using iterator_type = typename std::iterator_traits<iterator>::iterator_category;
		using is_random_access = std::is_base_of<std::random_access_iterator_tag, iterator_type>;
		using is_bidirectional = std::is_base_of<std::bidirectional_iterator_tag, iterator_type>;
		using reference = typename std::iterator_traits<iterator>::reference;

	private:
		iterator m_begin, m_end;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="range"/> class.
		/// </summary>
		/// <param name="begin">The begin iterator.</param>
		/// <param name="end">The end iterator.</param>
		range(const iterator & begin, const iterator & end)
			: m_begin(begin), m_end(end)
		{}

		/// <summary>
		/// Array access operator for random access iterators.
		/// </summary>
		/// <param name="index">The index.</param>
		/// <returns></returns>
		template <typename = std::enable_if<is_random_access::value>::type>
		reference operator[](size_t index) const
		{
			assert((long long)(index) < (m_end - m_begin));
			return m_begin[index];
		}

		/// <summary>
		/// An iterator on the first element.
		/// </summary>
		/// <returns></returns>
		iterator begin() const { return m_begin; }

		/// <summary>
		/// An iterator pass the last element.
		/// </summary>
		/// <returns></returns>
		iterator end() const { return m_end; }

		/// <summary>
		/// The first element of the range.
		/// </summary>
		/// <returns></returns>
		reference front() const { return *begin(); }

		/// <summary>
		/// The last element of the range (only for bidirectional iterators)
		/// </summary>
		/// <returns></returns>
		template <typename = std::enable_if<is_bidirectional::value>::type>
		reference back() const { return *std::prev(end()); }

		/// <summary>
		/// Determines whether this range is empty.
		/// </summary>
		/// <returns>
		///   <c>true</c> if this range is empty; otherwise, <c>false</c>.
		/// </returns>
		bool isEmpty() const
		{
			return m_begin == m_end;
		}

		/// <summary>
		/// Gets a reverse iterator on the last element of the sequence.
		/// </summary>
		/// <returns></returns>
		template <typename = std::enable_if<is_bidirectional::value>::type>
		std::reverse_iterator<iterator> rbegin() const { return std::make_reverse_iterator(std::prev(m_end)); }

		/// <summary>
		/// Gets an iterator past the last element in reverse order.
		/// </summary>
		/// <returns></returns>
		template <typename = std::enable_if<is_bidirectional::value>::type>
		std::reverse_iterator<iterator> rend() const { return std::make_reverse_iterator(std::prev(m_begin)); }
	};

	namespace internal_range
	{
		template <typename data_structure,
			typename iterator,
			typename method = decltype(std::declval<data_structure>().begin()),
			typename method2 = decltype(std::declval<data_structure>().end()),
			typename = std::is_constructible<iterator, method>,
			typename = std::is_constructible<iterator, method2>,
			typename = iterator::reference,
			typename = iterator::iterator_category
		>
			struct is_range_compatible
		{};

		template <
			typename iterator,
			typename = std::iterator_traits<iterator>::reference,
			typename = std::iterator_traits<iterator>::iterator_category
		>
		struct is_iterator_compatible
		{};
	}

	/// <summary>
	/// Creates a range with provided iterators.
	/// </summary>
	/// <param name="begin">The begin iterator.</param>
	/// <param name="end">The end iterator.</param>
	/// <returns></returns>
	template <typename iterator, typename = internal_range::is_iterator_compatible<iterator>>
	range<iterator> make_range(iterator begin, iterator end)
	{
		return range<iterator>(begin, end);
	}

	/// <summary>
	/// Creates a range from a data structure. If the instantiation fails, the provided types does not meet the requirements.
	/// </summary>
	/// <param name="data">The data structure.</param>
	/// <returns></returns>
	template <typename data_structure, typename = internal_range::is_range_compatible<data_structure, data_structure::iterator>>
	range<typename data_structure::iterator> make_range(data_structure & data)
	{
		return make_range(data.begin(), data.end());
	}

	/// <summary>
	/// Makes a range from the data structure. If the instantiation fails, the provided types does not meet the requirements.
	/// </summary>
	/// <param name="data">The data structure.</param>
	/// <returns></returns>
	template <typename data_structure, typename = internal_range::is_range_compatible<data_structure, data_structure::const_iterator>>
	range<typename data_structure::const_iterator> make_range(const data_structure & data)
	{
		return make_range(data.begin(), data.end());
	}
}