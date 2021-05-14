#pragma once

#include <unordered_map>
#include <deque>
#include <stdext/hash.h>
#include <cassert>

namespace AI
{
	/// <summary>
	/// A black board class. In a black board, you can store any kind of data. A piece of information is identified by its type and an associated
	/// string identifier. To accelerate requests, the string identifier must first be converted into a numerical identifier by using the method 
	/// <see cref="Blackboard::getId"> getId </see>.
	/// </summary>
	class Blackboard
	{
		/// <summary>
		/// Gets the destroy map storage. The destroy map is used to store callbacks used to remove all data associated with an instance of Blackboard
		/// </summary>
		/// <returns></returns>
		static std::unordered_map<const Blackboard *, std::deque<std::function<void()>>*> & getDestroyMapStorage()
		{
			static std::unordered_map<const Blackboard *, std::deque<std::function<void()>>*> destroyCallbacks;
			return destroyCallbacks;
		}

		/// <summary>
		/// Gets the destroy map i.e. the vector containing the callback functions used to clear an instance of blackboard.
		/// </summary>
		/// <param name="blackboard">The blackboard.</param>
		/// <returns></returns>
		static std::deque<std::function<void()>> & getDestroyMap(const Blackboard * blackboard)
		{
			std::unordered_map<const Blackboard *, std::deque<std::function<void()>>*> & destroyCallbacks = getDestroyMapStorage();
			auto it = destroyCallbacks.find(blackboard);
			if (it == destroyCallbacks.end())
			{
				std::deque<std::function<void()>> * tmp = new std::deque<std::function<void()>>();
				destroyCallbacks[blackboard] = tmp;
				return *tmp;
			}
			return *(it->second);
		}

		/// <summary>
		/// Removes the entry associated with a black board in the destroy map storage.
		/// </summary>
		/// <param name="blackboard">The blackboard.</param>
		static void removeDestroyEntry(const Blackboard * blackboard)
		{
			std::unordered_map<const Blackboard *, std::deque<std::function<void()>>*> & destroyCallbacks = getDestroyMapStorage();
			auto it = destroyCallbacks.find(blackboard);
			assert(it != destroyCallbacks.end());
			delete it->second;
			destroyCallbacks.erase(it);
		}

		/// <summary>
		/// Gets the map used to store blackboards data of type Type.
		/// </summary>
		/// <returns></returns>
		template <typename Type>
		static std::unordered_map<const Blackboard*, std::deque<Type> *> & getBlackboardMap()
		{
			static std::unordered_map<const Blackboard*, std::deque<Type> *> blackboards;
			return blackboards;
		}

		/// <summary>
		/// Gets the storage used to store data of type Type for the provided blackboard.
		/// </summary>
		/// <param name="blackboard">The blackboard.</param>
		/// <returns></returns>
		template <typename Type>
		static std::deque<Type> & getBlackboardData(const Blackboard * blackboard)
		{
			std::unordered_map<const Blackboard*, std::deque<Type> *> & map = getBlackboardMap<Type>();
			auto it = map.find(blackboard);
			if (it == map.end())
			{
				// We create the new storage and record it
				std::deque<Type> * tmp = new std::deque<Type>();
				map[blackboard] = tmp;
				// We register the destroy function for this data
				std::deque<std::function<void()>> & destroyCallbacks = getDestroyMap(blackboard);
				auto destroyFunction = [blackboard]()
				{
					std::unordered_map<const Blackboard*, std::deque<Type> *> & map = getBlackboardMap<Type>();
					auto it = map.find(blackboard);
					assert(it != map.end());
					delete it->second;
					map.erase(it);
				};
				destroyCallbacks.push_back(destroyFunction);
				// Now, we can return the storage data.
				return *tmp;
			}
			return *it->second;
		}

		/// <summary>
		/// Gets the id map used by the provided blackboard for data of type Type.
		/// </summary>
		/// <returns></returns>
		template <typename Type>
		static std::unordered_map<const Blackboard *, std::unordered_map<std::string, size_t> *> & getBlackboardIdMap()
		{
			static std::unordered_map<const Blackboard *, std::unordered_map<std::string, size_t> *> ids;
			return ids;
		}

		/// <summary>
		/// Gets the identifier map used for data of type Type by the provided blackboard.
		/// </summary>
		/// <param name="blackboard">The blackboard.</param>
		/// <returns></returns>
		template <typename Type>
		static std::unordered_map<std::string, size_t> & getBlackboardIdData(const Blackboard * blackboard)
		{
			std::unordered_map<const Blackboard *, std::unordered_map<std::string, size_t> *> & map = getBlackboardIdMap<Type>();
			auto it = map.find(blackboard);
			if (it == map.end())
			{
				// We create the new map
				std::unordered_map<std::string, size_t> * tmp = new std::unordered_map<std::string, size_t>();
				map[blackboard] = tmp;
				// We register the destroy function
				auto destroyFunction = [blackboard]()
				{
					std::unordered_map<const Blackboard *, std::unordered_map<std::string, size_t> *> & map = getBlackboardIdMap<Type>();
					auto it = map.find(blackboard);
					assert(it != map.end());
					delete it->second;
					map.erase(it);
				};
				getDestroyMap(blackboard).push_back(destroyFunction);
				// We return the result
				return *tmp;
			}
			return *it->second;
		}

	public:
		/// <summary>
		/// Read only access to blackboard data.
		/// </summary>
		template <class Type>
		class ConstBlackboardData
		{
			friend class Blackboard;

		protected:
			const Blackboard * m_blackboard;
			Type * m_data;

			/// <summary>
			/// Constructor dedicated to the blackboard.
			/// </summary>
			/// <param name="blackboard">The blackboard.</param>
			/// <param name="data">The data.</param>
			ConstBlackboardData(const Blackboard * blackboard, Type * data)
				: m_blackboard(blackboard), m_data(data)
			{}

		public:
			/// <summary>
			/// Default copy constructor.
			/// </summary>
			ConstBlackboardData(const ConstBlackboardData &) = default;

			/// <summary>
			/// Default assignment operator.
			/// </summary>
			ConstBlackboardData & operator = (const ConstBlackboardData &) = default;

			/// <summary>
			/// Cast oeprator to const Type&amp;.
			/// </summary>
			operator const Type & () const { return *m_data; }

			/// <summary>
			/// Accesses data designated by this instance.
			/// </summary>
			/// <returns></returns>
			const Type & operator * () const { return *m_data; }
		};

		/// <summary>
		/// Read / write access to blackboard data.
		/// </summary>
		template <typename Type>
		class BlackboardData : public ConstBlackboardData<Type>
		{
			friend class Blackboard;
		protected:

			/// <summary>
			/// Protected constructor dedicated to Blackboard.
			/// </summary>
			/// <param name="blackboard">The blackboard.</param>
			/// <param name="data">The data.</param>
			BlackboardData(const Blackboard * blackboard, Type * data)
				: ConstBlackboardData(blackboard, data)
			{}

		public:
			/// <summary>
			/// Default copy constructor.
			/// </summary>
			/// <param name="">The .</param>
			BlackboardData(const BlackboardData &) = default;

			/// <summary>
			///Default assignment operator.
			/// </summary>
			BlackboardData & operator = (const BlackboardData &) = default;

			/// <summary>
			/// Cast operator to Type&amp;.
			/// </summary>
			operator Type & () { return *m_data; }

			/// <summary>
			/// Accesses data designated by this instance.
			/// </summary>
			/// <returns></returns>
			Type & operator * () { return *m_data; }

			/// <summary>
			/// Changes the value designated by this operator
			/// </summary>
			/// <param name="value">The new value.</param>
			/// <returns></returns>
			BlackboardData & operator = (const Type & value)
			{
				(*m_data) = value;
				return (*this);
			}
		};

		/// <summary>
		/// Initializes a new instance of the <see cref="Blackboard"/> class.
		/// </summary>
		Blackboard()
		{}

		/// <summary>
		/// Finalizes an instance of the <see cref="Blackboard"/> class.
		/// </summary>
		~Blackboard()
		{
			clear();
		}

		/// <summary>
		/// Clears this blackboard. 
		/// </summary>
		void clear()
		{
			std::deque<std::function<void()>> & destroyCallbacks = getDestroyMap(this);
			for (auto it = destroyCallbacks.begin(), end = destroyCallbacks.end(); it != end; ++it)
			{
				(*it)(); // We call all destroy functions
			}
			removeDestroyEntry(this);
		}

		/// <summary>
		/// Gets the id of the variable of type Type identified by the provided identifier.
		/// </summary>
		/// <param name="identifier">The identifier.</param>
		/// <returns></returns>
		template <typename Type>
		BlackboardData<Type> get(const std::string & identifier, Type defaultValue = Type()) const
		{
			std::unordered_map<std::string, size_t> & ids = getBlackboardIdData<Type>(this);
			auto it = ids.find(identifier);
			std::deque<Type> & data = getBlackboardData<Type>(this);
			if (it == ids.end())
			{				
				data.push_back(defaultValue);
				ids[identifier] = data.size() - 1;
				//return data.size() - 1;
				return BlackboardData<Type>(this, &data.back());
			}
			return BlackboardData<Type>(this, &data[it->second]);
		}
	};
}