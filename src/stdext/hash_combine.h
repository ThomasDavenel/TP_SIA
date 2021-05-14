#pragma once

namespace stdext
{
	/// <summary>
	/// Combines the hash values associated with the provided objects.
	/// </summary>
	/// <param name="...args">The objects for which a combined hash value should be computed.</param>
	/// <returns></returns>
	template <typename... Type>
	size_t hash_combine(Type... args)
	{
		std::initializer_list<size_t> hashes = { std::hash<Type>()(args)... };
		size_t result = 0;
		for (const size_t * it = hashes.begin(), *end = hashes.end(); it != end; ++it)
		{
			size_t value = (*it);
			result ^= value + 0x9e3779b9 + (result << 6) + (result >> 2);
		}
		return result;
	}
}