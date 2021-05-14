#pragma once

namespace AI
{
	namespace Tasks
	{ 
		namespace internal
		{
			template <typename Clock = std::chrono::high_resolution_clock>
			class WaitFor : public Task
			{
				static_assert(stdext::chrono::is_clock<Clock>::value, "The provided class is not compatible with the STL clock type.");

			protected:
				typename Clock::duration m_duration;
				Status m_result;
				stdext::chrono::timer<Clock> m_timer;

				virtual void initialize() override
				{
					m_timer.start();
				}

				virtual Task::Status activity() override
				{
					m_timer.stop();
					if (m_timer.elapsed_time() >= m_duration)
					{
						return m_result;
					}
					return Status::running;
				}

				virtual void finalize() override {}

			public:
				WaitFor(double seconds, Status result = Status::success)
					: m_result(result)
				{
					m_duration = std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double, std::chrono::seconds::period>(seconds));
				}

				virtual WaitFor * clone() const
				{
					return new WaitFor(*this);
				}
			};

			template <typename Clock = std::chrono::high_resolution_clock>
			class TryDuring : public Task
			{
				static_assert(stdext::chrono::is_clock<Clock>::value, "The provided class is not compatible with the STL clock type.");

				typename Clock::duration m_duration;
				Status m_result;
				stdext::chrono::timer<Clock> m_timer;
				std::shared_ptr<Task> m_task;

				virtual void initialize() override
				{
					m_timer.start();
				}

				virtual Status activity() override
				{
					m_timer.stop();
					if (m_timer.elapsed_time() >= m_duration)
					{
						return m_result;
					}
					Status result = m_task->execute();
					if (result == Status::success || result == Status::failure)
					{
						return result;
					}
					return Status::running;
				}

				virtual void finalize()
				{
					m_task->reset();
				}

			public:
				TryDuring(double seconds, const std::shared_ptr<Task> & task, Status failResult = Status::failure)
					: m_result(failResult), m_task(task)
				{
					m_duration = std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double, std::chrono::seconds::period>(seconds));
				}

				virtual TryDuring * clone() const override
				{
					double duration = std::chrono::duration_cast<std::chrono::duration<double, std::chrono::seconds::period>>(m_duration).count();
					return new TryDuring(duration, std::shared_ptr<Task>(m_task->clone()), m_result);
				}
			};
		}
	}
}