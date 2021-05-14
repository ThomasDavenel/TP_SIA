#pragma once

#include <chrono>
#include <functional>
#include <type_traits>

namespace stdext
{
	namespace chrono
	{
		/// <summary>
		/// A simulated clock compatible with STL clocks and functionalities.
		/// </summary>
		class simulated_clock
		{
		public:
			using duration = std::chrono::nanoseconds;
			using period = std::chrono::nanoseconds::period;
			using rep = std::chrono::nanoseconds::rep;
			using time_point = std::chrono::time_point<simulated_clock>;

			using nanoseconds = std::chrono::nanoseconds;
			using seconds = std::chrono::duration<double, std::chrono::seconds::period>;

		protected:
			static time_point m_time;

		public:
			static constexpr bool is_steady() { return false; }

			/// <summary>
			/// Gets the current time.
			/// </summary>
			/// <returns></returns>
			static time_point now()
			{
				return m_time;
			}

			/// <summary>
			/// Updates the clock with the specified duration.
			/// </summary>
			/// <param name="duration">The duration.</param>
			template <typename Duration = seconds>
			static void update(const Duration & duration)
			{
				m_time += std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
			}

			/// <summary>
			/// Updates the clock with the specified time in seconds.
			/// </summary>
			/// <param name="timeInSeconds">The time in seconds.</param>
			static void update(double timeInSeconds)
			{
				update(seconds(timeInSeconds));
			}
		};
	}
}