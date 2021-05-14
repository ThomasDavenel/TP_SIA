#include <AI/Tasks/Task.h>
#include <vector>
#include <random>

namespace AI
{
	namespace Tasks
	{
		namespace internal
		{
			template <typename Condition>
			struct is_condition
			{
				static constexpr bool value = std::is_invocable<Condition>::value && std::is_assignable<bool&, decltype(std::declval<Condition>()())>::value;
			};

			class IfSuccessOrFailure : public Task
			{
			protected:
				enum class State { runningTask, runningTaskSuccess, runningTaskFailure };

				std::shared_ptr<Task> m_task;
				std::shared_ptr<Task> m_successTask;
				std::shared_ptr<Task> m_failureTask;
				State m_state;

				virtual void initialize() override
				{
					m_state = State::runningTask;
				}

				virtual Status activity() override
				{
					if (m_state == State::runningTask)
					{
						Status tmp = m_task->execute();
						if (tmp == Status::success)
						{
							m_state = State::runningTaskSuccess;
							return Status::running;
						}
						else if (tmp == Status::failure)
						{
							m_state = State::runningTaskFailure;
							return Status::running;
						}
						return tmp;
					}
					else if (m_state == State::runningTaskSuccess)
					{
						return m_successTask->execute();
					}
					else
					{
						return m_failureTask->execute();
					}
				}

				virtual void finalize() override
				{
					m_task->reset();
					m_successTask->reset();
					m_failureTask->reset();
				}

			public:
				IfSuccessOrFailure(const std::shared_ptr<Task> & task, const std::shared_ptr<Task> & successTask, const std::shared_ptr<Task> & failureTask)
					: m_task(task), m_successTask(successTask), m_failureTask(failureTask), m_state(State::runningTask)
				{}

				virtual IfSuccessOrFailure * clone() const override
				{
					return new IfSuccessOrFailure(std::shared_ptr<Task>(m_task->clone()), std::shared_ptr<Task>(m_successTask->clone()), std::shared_ptr<Task>(m_failureTask->clone()));
				}

				virtual ~IfSuccessOrFailure()
				{
					m_task->reset();
					m_successTask->reset();
					m_failureTask->reset();
				}
			};

			class Sequence : public Task
			{
				std::vector<std::shared_ptr<Task>> m_tasks;
				size_t m_currentTask;

				virtual void initialize() override
				{
					m_currentTask = 0;
				}

				virtual Status activity() override
				{
					Status tmp = m_tasks[m_currentTask]->execute();
					if (tmp == Status::failure) { return Status::failure; }
					if (tmp == Status::success) { m_currentTask++; }
					if (m_currentTask == m_tasks.size()) { return Status::success; }
					return Status::running;
				}

				virtual void finalize() override
				{
					for (size_t cpt = 0; cpt <= std::min(m_currentTask, m_tasks.size() - 1); ++cpt)
					{
						m_tasks[cpt]->reset();
					}
				}

			public:
				Sequence(const std::vector<std::shared_ptr<Task>> & tasks)
					: m_tasks(tasks)
				{}

				virtual Sequence * clone() const  override
				{
					std::vector<std::shared_ptr<Task>> clones;
					clones.reserve(m_tasks.size());
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						clones.push_back(std::shared_ptr<Task>((*it)->clone()));
					}
					return new Sequence(clones);
				}

				virtual ~Sequence()
				{
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						(*it)->reset();
					}
				}
			};

			template <typename ConditionType>
			class Precondition : public Task
			{
				//static_assert(std::is_invocable<ConditionType>::value, "ConditionType should be invocable with no parameter");
				//static_assert(std::is_assignable<bool&, decltype(std::declval<ConditionType>()())>::value, "ConditionType should return bool");
				static_assert(is_condition<ConditionType>::value, "ConditionType should be of type bool fnc()");

				ConditionType m_condition;
				std::shared_ptr<Task> m_task;
				std::shared_ptr<Task> m_running;

				virtual void initialize() override
				{
					m_running = std::shared_ptr<Task>();
				}

				virtual Status activity() override
				{
					if (!m_running && m_condition())
					{
						m_running = m_task;
					}
					if (m_running)
					{
						return m_running->execute();
					}
					return Status::sleeping;
				}

				virtual void finalize() override
				{
					if (m_running) { m_running->reset(); }
				}

			public:
				Precondition(const ConditionType & condition, const std::shared_ptr<Task> & task)
					: m_condition(condition), m_task(task)
				{}

				virtual Precondition * clone() const  override
				{
					return new Precondition(m_condition, std::shared_ptr<Task>(m_task->clone()));
				}

				virtual ~Precondition()
				{
					m_task->reset();
				}
			};

			class FirstOf : public Task
			{
			protected:
				std::vector<std::shared_ptr<Task>> m_tasks;
				std::shared_ptr<Task> m_current;

				virtual void initialize() override
				{
					m_current = std::shared_ptr<Task>();
				}

				virtual Status activity() override
				{
					if (!m_current)
					{
						for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
						{
							Status tmp = (*it)->execute();
							if (tmp != Status::sleeping)
							{
								m_current = (*it);
								return tmp;
							}
							else
							{
								m_current->reset();
							}
						}
					}
					else
					{
						return m_current->execute();
					}
					return Status::sleeping;
				}

				virtual void finalize() override
				{
					if (m_current) { m_current->reset(); }
				}

			public:
				FirstOf(const std::vector<std::shared_ptr<Task>> & tasks)
					: m_tasks(tasks)
				{}

				virtual FirstOf * clone() const override
				{
					std::vector<std::shared_ptr<Task>> clones;
					clones.reserve(m_tasks.size());
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						clones.push_back(std::shared_ptr<Task>((*it)->clone()));
					}
					return new FirstOf(clones);
				}

				~FirstOf()
				{
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						(*it)->reset();
					}
				}
			};

			class RandomPick : public FirstOf
			{
			protected:
				virtual void initialize() override
				{
					FirstOf::initialize();
					std::random_device rd;
					std::mt19937 g(rd());
					std::shuffle(m_tasks.begin(), m_tasks.end(), g);
				}

			public:
				RandomPick(const std::vector<std::shared_ptr<Task>> & tasks)
					: FirstOf(tasks)
				{}

				virtual RandomPick * clone() const override
				{
					std::vector<std::shared_ptr<Task>> clones;
					clones.reserve(m_tasks.size());
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						clones.push_back(std::shared_ptr<Task>((*it)->clone()));
					}
					return new RandomPick(clones);
				}
			};

			template <class Condition>
			class SucceedsWhen : public Task
			{
				static_assert(is_condition<Condition>::value, "Condition should be of type bool fnc()");

			protected:
				Condition m_condition;
				std::shared_ptr<Task> m_task;

				virtual void initialize() override
				{}

				virtual Status activity() override
				{
					if (m_condition()) { return Status::success; }
					Status tmp = m_task->execute();
					if (tmp == Status::failure) { return Status::failure; }
					return Status::running;
				}

				virtual void finalize() override
				{
					m_task->reset();
				}

			public:
				SucceedsWhen(const Condition & condition, const std::shared_ptr<Task> & task)
					: m_condition(condition), m_task(task)
				{}

				virtual SucceedsWhen * clone() const override
				{
					return new SucceedsWhen(m_condition, std::shared_ptr<Task>(m_task->clone()));
				}

				virtual ~SucceedsWhen()
				{
					m_task->reset();
				}
			};

			template <class Condition>
			class FailsWhen : public Task
			{
				static_assert(is_condition<Condition>::value, "Condition should be of type bool fnc()");

			protected:
				Condition m_condition;
				std::shared_ptr<Task> m_task;

				virtual void initialize() override
				{}

				virtual Status activity() override
				{
					if (m_condition()) { return Status::failure; }
					Status tmp = m_task->execute();
					if (tmp == Status::success) { return Status::success; }
					return Status::running;
				}

				virtual void finalize() override
				{
					m_task->reset();
				}

			public:
				FailsWhen(const Condition & condition, const std::shared_ptr<Task> & task)
					: m_condition(condition), m_task(task)
				{}

				virtual FailsWhen * clone() const override
				{
					return new FailsWhen(m_condition, std::shared_ptr<Task>(m_task->clone()));
				}

				virtual ~FailsWhen()
				{
					m_task->reset();
				}
			};

			class RepeatUntilSuccess : public Task
			{
				std::shared_ptr<Task> m_task;

				virtual void initialize() override
				{}

				virtual Status activity() override
				{
					Status tmp = m_task->execute();
					if (tmp == Status::success)
					{
						return Status::success;
					}
					return Status::running;
				}

				virtual void finalize()
				{
					m_task->reset();
				}

			public:
				RepeatUntilSuccess(const std::shared_ptr<Task> & task)
					: m_task(task)
				{}

				virtual RepeatUntilSuccess * clone() const override
				{
					return new RepeatUntilSuccess(std::shared_ptr<Task>(m_task->clone()));
				}

				virtual ~RepeatUntilSuccess()
				{
					m_task->reset();
				}
			};

			class RepeatUntilFailure : public Task
			{
				std::shared_ptr<Task> m_task;

				virtual void initialize() override
				{}

				virtual Status activity() override
				{
					Status tmp = m_task->execute();
					if (tmp == Status::failure)
					{
						return Status::success;
					}
					return Status::running;
				}

				virtual void finalize()
				{
					m_task->reset();
				}

			public:
				RepeatUntilFailure(const std::shared_ptr<Task> & task)
					: m_task(task)
				{}

				virtual RepeatUntilFailure * clone() const override
				{
					return new RepeatUntilFailure(std::shared_ptr<Task>(m_task->clone()));
				}

				virtual ~RepeatUntilFailure()
				{
					m_task->reset();
				}
			};

			class ParallelAnd : public Task
			{
				std::vector<std::shared_ptr<Task>> m_tasks;
				std::vector<std::shared_ptr<Task>> m_runningTasks;

				virtual void initialize() override
				{
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						m_runningTasks.push_back(*it);
					}
				}

				virtual Status activity() override
				{
					Status result = Status::running;
					for (size_t cpt = 0; cpt < m_runningTasks.size(); )
					{
						Status tmp = m_runningTasks[cpt]->execute();
						if (tmp == Status::success)
						{
							m_runningTasks[cpt] = std::move(m_runningTasks.back());
							m_runningTasks.pop_back();
						}
						else if (tmp == Status::failure)
						{
							m_runningTasks[cpt] = std::move(m_runningTasks.back());
							m_runningTasks.pop_back();
							result = Status::failure;
						}
						else
						{
							++cpt;
						}
					}
					if (m_runningTasks.size() == 0 && result != Status::failure) { return Status::success; }
					return result;
				}

				virtual void finalize() override
				{
					for (auto it = m_runningTasks.begin(), end = m_runningTasks.end(); it != end; ++it)
					{
						(*it)->reset();
					}
					m_runningTasks.erase(m_runningTasks.begin(), m_runningTasks.end());
				}

			public:
				ParallelAnd(const std::vector<std::shared_ptr<Task>> & tasks)
					: m_tasks(tasks)
				{
					m_runningTasks.reserve(m_tasks.size());
				}

				virtual ParallelAnd * clone() const override
				{
					std::vector<std::shared_ptr<Task>> clones;
					clones.reserve(m_tasks.size());
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						clones.push_back(std::shared_ptr<Task>((*it)->clone()));
					}
					return new ParallelAnd(clones);
				}

				virtual ~ParallelAnd()
				{
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						(*it)->reset();
					}
				}
			};

			class ParallelOr : public Task
			{
				std::vector<std::shared_ptr<Task>> m_tasks;
				std::vector<std::shared_ptr<Task>> m_runningTasks;

				virtual void initialize() override
				{
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						m_runningTasks.push_back(*it);
					}
				}

				virtual Status activity() override
				{
					Status result = Status::running;
					for (size_t cpt = 0; cpt < m_runningTasks.size(); )
					{
						Status tmp = m_runningTasks[cpt]->execute();
						if (tmp == Status::success)
						{
							m_runningTasks[cpt] = std::move(m_runningTasks.back());
							m_runningTasks.pop_back();
							result = Status::success;
						}
						else if (tmp == Status::failure)
						{
							m_runningTasks[cpt] = std::move(m_runningTasks.back());
							m_runningTasks.pop_back();
						}
						else
						{
							++cpt;
						}
					}
					if (m_runningTasks.size() == 0 && result == Status::running) { return Status::failure; }
					return result;
				}

				virtual void finalize() override
				{
					for (auto it = m_runningTasks.begin(), end = m_runningTasks.end(); it != end; ++it)
					{
						(*it)->reset();
					}
					m_runningTasks.erase(m_runningTasks.begin(), m_runningTasks.end());
				}

			public:
				ParallelOr(const std::vector<std::shared_ptr<Task>> & tasks)
					: m_tasks(tasks)
				{
					m_runningTasks.reserve(m_tasks.size());
				}

				virtual ParallelOr * clone() const override
				{
					std::vector<std::shared_ptr<Task>> clones;
					clones.reserve(m_tasks.size());
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						clones.push_back(std::shared_ptr<Task>((*it)->clone()));
					}
					return new ParallelOr(clones);
				}

				virtual ~ParallelOr()
				{
					for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it)
					{
						(*it)->reset();
					}
				}
			};
		}
	}
}