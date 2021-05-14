#pragma once
#include <chrono>
#include <AI/Tasks/Task.h>
#include <stdext/chrono/timer.h>
#include <AI/Tasks/internal/TimeOperators_imp.h>
#include <stdext/chrono/is_clock.h>

namespace AI
{
	namespace Tasks
	{

		/// <summary>
		/// Waits for the number of provided seconds.
		/// </summary>
		/// <param name="seconds">The number of seconds to wait.</param>
		/// <param name="result">The result.</param>
		/// <returns></returns>
		template <typename Clock = std::chrono::high_resolution_clock>
		std::shared_ptr<Task> waitFor(double seconds, Task::Status result = Task::Status::success)
		{
			static_assert(stdext::chrono::is_clock<Clock>::value, "The provided class is not compatible with the STL clock type.");
			return std::shared_ptr<Task>(new internal::WaitFor<Clock>(seconds, result));
		}

		/// <summary>
		/// Tries to perform a task during a given amount of time. If the task succeeds or fails during the provided amount of time,
		/// its result is returns. If the task is not finished, the provided result is returned.
		/// </summary>
		/// <param name="seconds">The seconds.</param>
		/// <param name="task">The task.</param>
		/// <param name="result">The result.</param>
		/// <returns></returns>
		template <typename Clock = std::chrono::high_resolution_clock>
		std::shared_ptr<Task> tryDuring(double seconds, const std::shared_ptr<Task> & task, Task::Status result = Task::Status::failure)
		{
			static_assert(stdext::chrono::is_clock<Clock>::value, "The provided class is not compatible with the STL clock type.");
			return std::shared_ptr<Task>(new internal::TryDuring<Clock>(seconds, task, result));
		}
	}
}