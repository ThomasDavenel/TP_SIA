#pragma once

#include <functional>

namespace std
{
	/// <summary>
	/// Hash function for type_info
	/// </summary>
	template <>
	struct hash<type_info>
	{
		size_t operator() (const type_info & t) const
		{
			return t.hash_code();
		}
	};

	/// <summary>
	/// Equal function for type_info
	/// </summary>
	template <>
	struct equal_to<type_info>
	{
		bool operator()(const type_info & o1, const type_info & o2) const
		{
			return o1 == o2;
		}
	};

	/// <summary>
	/// Hash function for reference on type_info
	/// </summary>
	template <>
	struct hash<std::reference_wrapper<const type_info>>
	{
		size_t operator() (const std::reference_wrapper<const type_info> & t) const
		{
			return t.get().hash_code();
		}
	};

	/// <summary>
	/// Equality function for reference on type_info
	/// </summary>
	template <>
	struct equal_to<std::reference_wrapper<const type_info>>
	{
		bool operator() (const std::reference_wrapper<const type_info> & o1, const std::reference_wrapper<const type_info> & o2) const
		{
			return o1.get() == o2.get();
		}
	};
}