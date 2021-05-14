#pragma once

#include <AI/Tasks/internal/Operators_imp.h>
#include <AI/Tasks/Task.h>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <random>

namespace AI
{
	namespace Tasks
	{
		/// <summary>
		/// And operator. If the first task succeeds, the second task is executed and the operator returns the status of the second task.
		/// If the first task fails, the operator fails.
		/// </summary>
		/// <param name="t1">The t1.</param>
		/// <param name="successTask">The success task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> operator && (const std::shared_ptr<Task> & t1, const std::shared_ptr<Task> & successTask)
		{
			return std::make_shared<internal::IfSuccessOrFailure>(t1, successTask, makeTask([]() { return Task::Status::failure; }));
		}

		/// <summary>
		/// Or operator. If the first task succeeds, the operators succeeds. If the first task fails, the operator returns the status
		/// of the second task.
		/// </summary>
		/// <param name="task">The task.</param>
		/// <param name="failureTask">The failure task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> operator || (const std::shared_ptr<Task> & task, const std::shared_ptr<Task> & failureTask)
		{
			return std::make_shared<internal::IfSuccessOrFailure>(task, makeTask([]() { return Task::Status::success; }), failureTask);
		}

		/// <summary>
		/// Executes the tasks in parallel. If a task fails, the operator fails.
		/// </summary>
		/// <param name="tasks">The tasks.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> parallelAnd(const std::vector<std::shared_ptr<Task>> & tasks)
		{
			return std::make_shared<internal::ParallelAnd>(tasks);
		}

		/// <summary>
		/// Executes tasks in parallel. If a tasks succeeds, the operator succeeds.
		/// </summary>
		/// <param name="tasks">The tasks.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> parallelOr(const std::vector<std::shared_ptr<Task>> & tasks)
		{
			return std::make_shared<internal::ParallelOr>(tasks);
		}

		/// <summary>
		/// If task succeeds, success task is performed. If task fails, the failure task is performed. The result of this operator
		/// is the success or failure of the second task.
		/// </summary>
		/// <param name="task">The task.</param>
		/// <param name="successTask">The success task.</param>
		/// <param name="failureTask">The failure task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> ifSuccessOrFailure(const std::shared_ptr<Task> & task, const std::shared_ptr<Task> & successTask, const std::shared_ptr<Task> & failureTask)
		{
			return std::make_shared<internal::IfSuccessOrFailure>(task, successTask, failureTask);
		}

		/// <summary>
		/// Realize the sequence of tasks. If a task fails, the whole sequence fails.
		/// </summary>
		/// <param name="tasks">The tasks.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> sequence(const std::vector<std::shared_ptr<Task>> & tasks)
		{
			return std::make_shared<internal::Sequence>(tasks);
		}

		/// <summary>
		/// Negates the result of the task i.e. succeeds if failes and reciprocally.
		/// </summary>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> negate(const std::shared_ptr<Task> & task)
		{
			auto fnc = [task]()
			{
				Task::Status tmp = task->execute();
				if (tmp == Task::Status::success) { return Task::Status::failure; }
				if (tmp == Task::Status::failure) { return Task::Status::success; }
				return tmp;
			};
			return makeTask(fnc);
		}

		/// <summary>
		/// Adds a precondition to the provided task. If the precondition is false, the execute call returns a sleeping status.
		/// </summary>
		/// <param name="precondition">The precondition.</param>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		template <typename PreconditionFnc>
		inline std::shared_ptr<Task> precondition(const PreconditionFnc & precondition, const std::shared_ptr<Task> & task)
		{
			static_assert(internal::is_condition<PreconditionFnc>::value, "PreconditionFnc should be of type bool fnc()");
			return std::make_shared<internal::Precondition<PreconditionFnc>>(precondition, task);
		}

		/// <summary>
		/// Execute the first task with a true precondition i.e. the first task that is not sleeping after a call to execute.
		/// </summary>
		/// <param name="tasks">The tasks.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> firstOf(const std::vector<std::shared_ptr<Task>> & tasks)
		{
			return std::make_shared<internal::FirstOf>(tasks);
		}

		/// <summary>
		/// Randomly picks a task to execute and executes the first found to be not sleeping after a call to execute.
		/// </summary>
		/// <param name="tasks">The set of tasks.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> randomPick(const std::vector<std::shared_ptr<Task>> & tasks)
		{
			return std::make_shared<internal::RandomPick>(tasks);
		}

		/// <summary>
		/// This operator changes the success condition of the task. The task is executed until the condition becomes true. If  the
		/// task fails, the operator fails.
		/// </summary>
		/// <param name="condition">The new success condition.</param>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		template <typename ConditionFnc>
		inline std::shared_ptr<Task> succeedsWhen(const ConditionFnc & condition, const std::shared_ptr<Task> & task)
		{
			static_assert(internal::is_condition<ConditionFnc>::value, "ConditionFnc should be of type bool fnc()");
			return std::make_shared<internal::SucceedsWhen<ConditionFnc>>(condition, task);
		}

		/// <summary>
		/// This operator changes the failure condition of the task. If the task succeeds, the operator succeeds. 
		/// </summary>
		/// <param name="condition">The condition.</param>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		template <typename ConditionFnc>
		inline std::shared_ptr<Task> failsWhen(const ConditionFnc & condition, const std::shared_ptr<Task> & task)
		{
			static_assert(internal::is_condition<ConditionFnc>::value, "ConditionFnc should be of type bool fnc()");
			return std::make_shared<internal::FailsWhen<ConditionFnc>>(condition, task);
		}

		/// <summary>
		/// The task always returns a success status even if the provided one failed.
		/// </summary>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> alwaysSucceeds(const std::shared_ptr<Task> & task)
		{
			return task || makeTask([]() { return Task::Status::success; });
		}

		/// <summary>
		/// The task always returns a  failure status, even if the provided one succeeded.
		/// </summary>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> alwaysFails(const std::shared_ptr<Task> & task)
		{
			return task && makeTask([]() { return Task::Status::failure; });
		}

		/// <summary>
		/// Repeats the task until it succeeds.
		/// </summary>
		/// <param name="task">The task to repeat.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> repeatUntilSuccess(const std::shared_ptr<Task> & task)
		{
			return std::make_shared<internal::RepeatUntilSuccess>(task);
		}

		/// <summary>
		/// Repeats the task until it fails, when this happens the operator returns success.
		/// </summary>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> repeatUntilFailure(const std::shared_ptr<Task> & task)
		{
			return std::make_shared<internal::RepeatUntilFailure>(task);
		}

		/// <summary>
		/// Loops indefinitely on the provided task. This task is always in running state.
		/// </summary>
		/// <param name="task">The task.</param>
		/// <returns></returns>
		inline std::shared_ptr<Task> loop(const std::shared_ptr<Task> & task)
		{
			auto fnc = [task]() { task->execute(); return Task::Status::running; };
			return makeTask(fnc);
		}
	}
}