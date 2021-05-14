#pragma once
#include <algorithm>
#include <limits>
#include <cassert>
#include <stdext/kmap.h>

static size_t s_vpTreeDistanceCount = 0;

namespace MotionPlanning
{
	/// <summary>
	/// Vantage point tree for nearest neighbor queries
	/// </summary>
	template <typename Data, typename SearchData=Data>
	class VPTree
	{
		/// <summary>
		/// The buckets size (maximum number of elements in the leaves).
		/// </summary>
		/// <returns></returns>
		static constexpr size_t bucketSize() { return 8; } // 16

		/// <summary>
		/// A node of vantage point tree
		/// </summary>
		class Node
		{
		protected:
			/// <summary>
			/// The m centroid
			/// </summary>
			Data m_centroid;
			/// <summary>
			/// The elements stored in the node
			/// </summary>
			::std::vector<Data> m_data;
			/// <summary>
			/// The actual radius of the node (covers all sub nodes)
			/// </summary>
			float m_radius;
			/// <summary>
			/// The distance limit used to separate inside and outside 
			/// </summary>
			float m_limit;
			/// <summary>
			/// The left node (node with elements which distance is lesser than m_limit)
			/// </summary>
			Node * m_left;
			/// <summary>
			/// The right node (node with elements which distance is greater than m_limit)
			/// </summary>
			Node * m_right;

		public:
			/// <summary>
			/// Initializes a new instance of the <see cref="VPTree"/> class.
			/// </summary>
			/// <param name="centroid">The centroid of the node.</param>
			Node(const Data & centroid)
				: m_centroid(centroid), m_radius(0), m_left(nullptr), m_right(nullptr), m_limit(std::numeric_limits<float>::max())
			{
				m_data.reserve(bucketSize());
			}

			/// <summary>
			/// Finalizes an instance of the <see cref="VPTree"/> class. Destroys all sub nodes 
			/// </summary>
			~Node()
			{
				if(m_left != nullptr) delete m_left;
				if(m_right != nullptr) delete m_right;
			}

			/// <summary>
			/// Collects the data in the tree.
			/// </summary>
			/// <param name="result">The collected data.</param>
			void collectData(std::vector<Data> & result)
			{
				result.push_back(m_centroid);
				for (auto it = m_data.begin(), end = m_data.end(); it != end; ++it)
				{
					result.push_back(*it);
				}
				if (m_left) { m_left->collectData(result); }
				if (m_right) { m_right->collectData(result);  }
			}

			/// <summary>
			/// Adds the specified value in the tree.
			/// </summary>
			/// <param name="value">The value to be added.</param>
			/// <param name="distance">The distance function.</param>
			template <typename DistanceFunction>
			void add(const Data & value, const DistanceFunction & distance)
			{
				m_radius = std::max(m_radius, distance(m_centroid, value));
				if (m_left && m_right)
				{
					if (distance(m_centroid, value) <= m_limit)
					{
						m_left->add(value, distance);
					}
					else
					{
						m_right->add(value, distance);
					}
				}
				else
				{
					m_data.push_back(value);
					if (m_data.size() == bucketSize())
					{
						size_t middleIndex = bucketSize() / 2 - 1;
						auto comp = [this, &distance](const Data & d1, const Data & d2) { return distance(m_centroid, d1) < distance(m_centroid, d2); };
						std::sort(m_data.begin(), m_data.end(), comp); // I should use nth_element, O(n) in average instead of O(n log2 n).
						//std::nth_element(m_data.begin(), m_data.begin() + middleIndex, m_data.end(), comp);
						m_limit = distance(m_centroid, m_data[middleIndex]);
						m_left = new Node(m_data.front());
						m_right = new Node(m_data.back());
						for (auto it = m_data.begin() + 1, end = m_data.end() - 1; it != end; ++it)
						{
							if (distance(m_centroid, *it) <= m_limit)
							{
								m_left->add(*it, distance);
							}
							else
							{
								m_right->add(*it, distance);
							}
						}
						m_data.clear();
					}
				}
			}

			/// <summary>
			/// Finds the nearest neighbor of value
			/// </summary>
			/// <param name="value">The value.</param>
			/// <param name="distance">The distance function distance(Data, SearchData).</param>
			/// <param name="nearestRadius">The nearest distance found so far.</param>
			/// <returns></returns>
			template <typename DistanceFunction>
			void nearestNeighbour(const SearchData & value, const DistanceFunction & distance, Data & nearest, float & nearestDistance)
			{
				float centroidDistance = distance(m_centroid, value);
				if(centroidDistance<nearestDistance)
				{
					nearest = m_centroid;
					nearestDistance = centroidDistance;
				}
				for (auto it = m_data.begin(), end = m_data.end(); it != end; ++it)
				{
					float localDistance = distance(*it, value);
					if(localDistance < nearestDistance)
					{
						nearest = (*it);
						nearestDistance = localDistance;
					}
				}
				if(m_left && distance(m_centroid, value)-nearestDistance<=m_limit)
				{
					m_left->nearestNeighbour(value, distance, nearest, nearestDistance);
				}
				if(m_right && distance(m_centroid, value)+nearestDistance>m_limit)
				{
					m_right->nearestNeighbour(value, distance, nearest, nearestDistance);
				}
			}
			
			/// <summary>
			/// Selects all the elements in a ball centered in center with radius radius.
			/// </summary>
			/// <param name="center">The center of the ball.</param>
			/// <param name="radius">The radius of the ball.</param>
			/// <param name="distance">The distance function.</param>
			/// <param name="result">The selected elements.</param>
			template <typename DistanceFunction>
			void select(const SearchData & center, double radius, const DistanceFunction & distance, std::vector<Data> & result)
			{
				const float distanceToCentroid = distance(m_centroid, center);
				// If the distance to the centroid is greater than the sum of this node radius and the provided radius, nothing to do...
				if (distanceToCentroid > radius + m_radius) { return; } 
				if(distanceToCentroid<=radius) // If the centroid is in the selector, we add it to the results;
				{
					result.push_back(m_centroid);
				}
				for(auto it=m_data.begin(), end = m_data.end() ; it!=end; ++it)
				{
					if(distance(*it, center)<=radius)
					{
						result.push_back(*it);
					}
				}
				if (m_left && distanceToCentroid - radius <= m_limit) { m_left->select(center, radius, distance, result); }
				else if (m_left) { m_left->debugDistance(center, radius, distance); }
				if (m_right && distanceToCentroid + radius > m_limit) { m_right->select(center, radius, distance, result); }
				else if (m_right) { m_right->debugDistance(center, radius, distance); }
			}

			template <typename DistanceFunction>
			void debugDistance(const SearchData & center, double radius, const DistanceFunction & distance)
			{
				//const float distanceToCentroid = distance(m_centroid, center);
				//assert(distanceToCentroid > radius);
				//for (auto it = m_data.begin(), end = m_data.end(); it != end; ++it)
				//{
				//	float distanceElement = distance(*it, center);
				//	if (distanceElement <= radius)
				//	{
				//		assert(false);
				//	}
				//}
				//if (m_left) { m_left->debugDistance(center, radius, distance); }
				//if (m_right) { m_right->debugDistance(center, radius, distance); }
			}

			/// <summary>
			/// Computes the k nearest neighbours of the provided value.
			/// </summary>
			/// <param name="center">The value for which the k nearest neighbours should be computed.</param>
			/// <param name="distance">The distance function distance(Data, SearchData).</param>
			/// <param name="result">The result.</param>
			template <typename DistanceFunction>
			void kNearestNeighbours(const SearchData & center, const DistanceFunction & distance, stdext::kmap<float, Data> & result)
			{
				const float distanceToCentroid = distance(m_centroid, center);
				float radius = std::numeric_limits<float>::max();
				if (result.has_max()) { radius = result.max(); }
				// No need to explore if the center is too far from the centroid
				if (distanceToCentroid - radius > m_radius) { return; }
				// If the distance to the centroid is lesser that the maximum distance, we add the centroid
				if (distanceToCentroid <= radius)
				{
					result.insert({ distanceToCentroid, m_centroid });
					if (result.has_max()) { radius = result.max(); } // We update the radius if needed
				}
				// We add all the data in the kmap structure if needed
				for (auto it = m_data.begin(), end = m_data.end(); it != end; ++it)
				{
					result.insert({distance(*it, center), *it});
				}
				if (result.has_max()) { radius = result.max(); } // We update the radius if needed
				// We explore the left son if needed
				if (m_left && distanceToCentroid - radius <= m_limit) 
				{ 
					m_left->kNearestNeighbours(center, distance, result); 
					if (result.has_max()) { radius = result.max(); } // We update the radius if needed
				}
				// We explore the right son if needed
				if (m_right && distanceToCentroid + radius > m_limit) 
				{ 
					m_right->kNearestNeighbours(center, distance, result); 
					if (result.has_max()) { radius = result.max(); } // We update the radius if needed
				}
			}

			/// <summary>
			/// Gets the centroid.
			/// </summary>
			/// <returns></returns>
			const Data & getCentroid() const { return m_centroid; }
		};

		/// <summary>
		/// The root node of the tree
		/// </summary>
		Node *  m_root;
		/// <summary>
		/// The number of elements in the tree
		/// </summary>
		size_t m_nbData;
		/// <summary>
		/// The number of elements in the tree during last reorganization
		/// </summary>
		size_t m_previousNbData;

		/// <summary>
		/// Reorganizes the tree. This method creates a more balanced tree.
		/// </summary>
		void reorganize()
		{
			if (m_root == nullptr || m_nbData != m_previousNbData) { return; } // Nothing to do
			m_previousNbData *= 2;
			recompute();
		}

		/// <summary>
		/// Adds the specified value in the tree.
		/// </summary>
		/// <param name="value">The value to be added.</param>
		/// <param name="distance">The distance function.</param>
		template <typename Distance>
		void add(const Data & value, const Distance & distance)
		{
			reorganize();
			if (m_root == nullptr) { m_root = new Node(value); }
			else { m_root->add(value, distance); }
			m_nbData++;
		}

		/// <summary>
		/// Finds the nearest neighbor.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <param name="distance">The distance function.</param>
		/// <returns></returns>
		template <typename Distance>
		Data nearestNeighbour(const SearchData & value, const Distance & distance) const
		{
			assert(m_root != nullptr);
			//return m_root->nearestNeighbour(value, distance);
			float nearestDistance = distance(m_root->getCentroid(), value);
			Data nearest = m_root->getCentroid();
			m_root->nearestNeighbour(value, distance, nearest, nearestDistance);
			return nearest;
		}

		/// <summary>
		/// Selects all nodes lying in the ball centered at value, of radius radius.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <param name="radius">The radius.</param>
		/// <param name="distance">The distance function.</param>
		/// <param name="result">The result.</param>
		template <typename Distance>
		void select(const SearchData & value, double radius, const Distance & distance, std::vector<Data> & result)
		{
			if (m_root == nullptr) { return; } // Empty tree.
			m_root->select(value, radius, distance, result);
		}

		/// <summary>
		/// Selects the k nearest neighbors.
		/// </summary>
		/// <param name="center">The center.</param>
		/// <param name="distance">The distance.</param>
		/// <param name="result">The result.</param>
		template <typename Distance>
		void kNearestNeighbour(const SearchData & center, const Distance & distance, stdext::kmap<float, Data> & result)
		{
			if (m_root == nullptr) { return; }
			m_root->kNearestNeighbours(center, distance, result);
		}

		/// <summary>
		/// Function used to add elements in the tree (trick to hide distance function type without big loss of performances)
		/// </summary>
		std::function<void(const Data &)> addFunction;

		/// <summary>
		/// Function used to find the nearest neighbor (trick to hide distance function type without big loss of performances)
		/// </summary>
		std::function<Data(const SearchData &)> neighbourFunction;

		/// <summary>
		/// Function used to find all the elements in a ball.
		/// </summary>
		std::function<void(const SearchData &, double radius, std::vector<Data> &)> selectFunction;

		std::function<void(const SearchData &, stdext::kmap<float, Data> &)> kNearestNeighbourFunction;
		
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="VPTree"/> class.
		/// </summary>
		/// <param name="distanceData">The distance function between data elements.</param>
		/// <param name="distanceSearch">The distance function between a data element and the type used to request a nearest neighbor.</param>
		template <typename Distance, typename DistanceSearch>
		VPTree(Distance distanceData, DistanceSearch distanceSearch)
			: m_root(nullptr), m_nbData(0), m_previousNbData(bucketSize() * 2)
		{
			addFunction = [this, distanceData](const Data & value) { add(value, distanceData); };
			neighbourFunction = [this, distanceSearch](const SearchData & value) -> Data { return nearestNeighbour(value, distanceSearch); };
			selectFunction = [this, distanceSearch](const SearchData & center, double radius, std::vector<Data> & result) { select(center, radius, distanceSearch, result); };
			kNearestNeighbourFunction = [this, distanceSearch](const SearchData & center, stdext::kmap<float, Data> & result) { kNearestNeighbour(center, distanceSearch, result); };
		}

		/// <summary>
		/// We delete the copy constructor
		/// </summary>
		/// <param name="">The .</param>
		VPTree(const VPTree &) = delete;

		/// <summary>
		/// We delete the assignment operator
		/// </summary>
		/// <param name="">The .</param>
		/// <returns></returns>
		VPTree & operator= (const VPTree &) = delete;

		/// <summary>
		/// Clears this tree.
		/// </summary>
		void clear()
		{
			if(m_root!=nullptr)
			{
				delete m_root;
				m_root = nullptr;
				m_nbData = 0;
				m_previousNbData = bucketSize() * 2;
			}
		}
		
		/// <summary>
		/// Finalizes an instance of the <see cref="VPTree"/> class.
		/// </summary>
		~VPTree()
		{
			if (m_root != nullptr) { delete m_root; }
		}

		/// <summary>
		/// Adds the specified value in the tree.
		/// </summary>
		/// <param name="value">The value.</param>
		void add(const Data & value)
		{
			addFunction(value);
		}

		/// <summary>
		/// Recomputes this tree. Useful if the elements are moving objects.
		/// </summary>
		void recompute()
		{
			std::vector<Data> collected;
			collected.reserve(m_nbData);
			m_root->collectData(collected);
			delete m_root;
			m_root = nullptr;
			m_nbData = 0;
			while (!collected.empty())
			{
				size_t index = rand() % collected.size();
				std::swap(collected[index], collected.back());
				add(collected.back());
				collected.pop_back();
			}
		}

		/// <summary>
		/// Computes the nearest neighbor.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <returns></returns>
		Data nearestNeighbour(const SearchData & value) const
		{
			return neighbourFunction(value);
		}

		/// <summary>
		/// Selects the elements in the ball centered in center with radius radius.
		/// </summary>
		/// <param name="center">The center of the ball.</param>
		/// <param name="radius">The radius of the ball.</param>
		/// <returns></returns>
		std::vector<Data> select(const SearchData & center, double radius) const
		{
			std::vector<Data> result;
			selectFunction(center, radius, result);
			return result;
		}

		/// <summary>
		/// Returns the k nearest neighbours of the provided value.
		/// </summary>
		/// <param name="center">The value for which the k nearest neighbours should be computed.</param>
		/// <param name="k">The k.</param>
		/// <returns></returns>
		std::vector<Data> kNearestNeighbour(const SearchData & center, size_t k)
		{
			if (m_root == nullptr) { return std::vector<Data>(); }
			stdext::kmap<float, Data> result(k);
			kNearestNeighbourFunction(center, result);
			std::vector<Data> toReturn;
			toReturn.reserve(k);
			for (auto it = result.begin(), end = result.end(); it != end; ++it)
			{
				toReturn.push_back(it->second);
			}
			return toReturn;
		}

		/// <summary>
		/// Returns the size of the tree
		/// </summary>
		/// <returns></returns>
		size_t size() const { return m_nbData; }
	};
}