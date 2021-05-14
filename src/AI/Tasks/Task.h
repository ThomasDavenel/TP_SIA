#pragma once

#include <memory>
#include <iostream>

namespace AI
{
	namespace Tasks
	{
		/// <summary>
		/// Base class for task description. This class is abstract.
		/// </summary>
		class Task
		{
		public:
			/// <summary>
			/// The execution status of a task.
			/// </summary>
			enum class Status { 
				/// <summary> The task succeeded </summary>
				success, 
				/// <summary> The task failed </summary>
				failure, 
				/// <summary> The task is running </summary>
				running, 
				/// <summary> The task is sleeping i.e. it has been initialized but it is waiting for something before starting its execution (to use if there is a precondition to a task). </summary>
				sleeping, 
				/// <summary> The task has been reset but not initialized yet. </summary>
				none
			};

		private:
			Status m_status;

		protected:
			Task()
				: m_status(Status::none)
			{}

		public:
			/// <summary>
			/// Gets the status.
			/// </summary>
			/// <returns></returns>
			Status getStatus() const { return m_status; }

		protected:
			/// <summary>
			/// Intializes the task instance.
			/// </summary>
			virtual void initialize() = 0;
			/// <summary>
			/// The activity of the task, called until the tasks succeeds or fails.
			/// </summary>
			/// <returns></returns>
			virtual Status activity() = 0;
			/// <summary>
			/// Called when the task has finished (success or failure)
			/// </summary>
			virtual void finalize() = 0;

		public:
			/// <summary>
			/// Executes this task. Once this task as succeeded or failed, its status is reset and a new call
			/// results in a reinitialization of the task and a new call forces a new execution.
			/// </summary>
			/// <returns></returns>
			Status execute()
			{
				if (m_status == Status::success || m_status == Status::failure)
				{
					m_status = Status::none;
				}
				if (m_status == Status::none)
				{
					initialize();
					m_status = Status::sleeping;
				}
				if (m_status == Status::running || m_status == Status::sleeping)
				{
					m_status = activity();
				}
				if (m_status == Status::success || m_status == Status::failure)
				{
					finalize();
				}
				return m_status;

				// Old version
				//if (m_status == Status::sleeping)
				//{
				//	initialize();
				//	m_status = Status::running;
				//}
				//if (m_status == Status::running)
				//{
				//	m_status = activity();
				//} 
				//if (m_status == Status::success || m_status == Status::failure)
				//{
				//	Status tmp = m_status;
				//	finalize();
				//	m_status = Status::sleeping;
				//	return tmp;
				//}
				//return m_status;
			}

			/// <summary>
			/// Resets this task.
			/// </summary>
			void reset()
			{
				if (m_status != Status::success && m_status != Status::failure && m_status != Status::none)
				{
					finalize();
				}
				if (m_status != Status::none)
				{
					m_status = Status::none;
				}
			}

			virtual Task * clone() const = 0;

			virtual ~Task() 
			{}
		};

		namespace internal
		{
			template <typename InitializationType, typename ActivityType, typename FinalizationType>
			class TemplatedTask : public Task
			{
				InitializationType m_initialize;
				ActivityType m_activity;
				FinalizationType m_finalize;

				static_assert(std::is_invocable<InitializationType>::value, "InitializationType should be invocable with no parameter");
				static_assert(std::is_invocable<FinalizationType>::value, "FinalizationType should be invocable with no parameter");
				static_assert(std::is_invocable<ActivityType>::value, "ActivityType should be invocable with no parameter");
				static_assert(std::is_assignable<Status &, decltype(std::declval<ActivityType>()())>::value, "Function ActivityType should return AI::Tasks::Task::Status");

			public:
				TemplatedTask(const InitializationType & initializeFnc, const ActivityType & activityFnc, const FinalizationType & finalizeFnc)
					: m_initialize(initializeFnc), m_activity(activityFnc), m_finalize(finalizeFnc)
				{}

				virtual TemplatedTask * clone() const override { return new TemplatedTask(*this); }

			protected:
				virtual void initialize() override { m_initialize(); }

				virtual Status activity() override { return m_activity(); }

				virtual void finalize() override { m_finalize(); }
			};
		}

		template <typename InitializationType, typename ActivityType, typename FinalizationType>
		std::shared_ptr<Task> makeTask(const InitializationType & init, const ActivityType & activity, const FinalizationType & finalization)
		{
			return std::shared_ptr<Task>(new internal::TemplatedTask<InitializationType, ActivityType, FinalizationType>(init, activity, finalization));
		}

		template <typename ActivityType>
		std::shared_ptr<Task> makeTask(const ActivityType & activity)
		{
			return makeTask([]() {}, activity, []() {});
		}

		inline std::ostream & operator<< (std::ostream & out, Task::Status status)
		{
			switch (status)
			{
			case Task::Status::failure:
				out << "failure";
				break;
			case Task::Status::running:
				out << "running";
				break;
			case Task::Status::sleeping:
				out << "sleeping";
				break;
			case Task::Status::success:
				out << "success";
				break;
			case Task::Status::none:
				out << "none";
				break;
			}
			return out;
		}
	}
}