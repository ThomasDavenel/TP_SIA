#pragma  once

#include <type_traits>
#include <functional>
#include <stdext/type_traits_helpers.h>

namespace stdext
{
	namespace chrono
	{
		/// <summary>
		/// Verifies if the provided type Clock can be used as a clock.
		/// </summary>
		template <typename Clock>
		struct is_clock
		{
		protected:
			macro_has_type(duration);
			macro_has_type(time_point);
			macro_has_type(rep);
			macro_has_type(period);
			macro_has_method(now, Clock::time_point);

		public:
			static constexpr bool value =
				has_duration<Clock>() &&
				has_time_point<Clock>() && 
				has_rep<Clock>() &&
				has_period<Clock>() &&
				has_now<Clock>();
		};
	}
}
