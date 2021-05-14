#pragma once

#include <chrono>
#include <stdext/chrono/is_clock.h>

namespace stdext
{
	namespace chrono
	{
		/// <summary>
		/// Implements a timer that enable to measure elapsed time.
		/// </summary>
		template <typename Clock = std::chrono::high_resolution_clock>
		class timer
		{
		public:
			static_assert(is_clock<Clock>::value, "You must provide a Clock type that is compatible with the STL clock type");

			using nanoseconds = std::chrono::duration<long long, std::chrono::nanoseconds::period>;
			using seconds = std::chrono::duration<double, std::chrono::seconds::period>;

		protected:
			nanoseconds m_start;
			nanoseconds m_end;

		public:
			/// <summary>
			/// Initializes a new instance of the <see cref="timer"/> class.
			/// </summary>
			timer()
				: m_start(0), m_end(0)
			{}

			/// <summary>
			/// Starts this timer.
			/// </summary>
			void start()
			{
				m_start = Clock::now().time_since_epoch();
			}

			/// <summary>
			/// Stops this timer i.e. measures the current time. Note that stop() can be called several times. In such a case
			/// the start time does not change.
			/// </summary>
			void stop()
			{
				m_end = Clock::now().time_since_epoch();
			}

			/// <summary>
			/// Gets the elapsed time between start() and stop().
			/// </summary>
			/// <returns></returns>
			template <typename Duration = seconds>
			Duration elapsed_time() const
			{
				return std::chrono::duration_cast<Duration>(m_end - m_start);
			}
		};
	}
}