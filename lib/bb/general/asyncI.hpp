
/* -------------------------------------------------------------------------- */

#ifndef __BB_GENERAL_ASYNC_I_HPP__
# define __BB_GENERAL_ASYNC_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <condition_variable>
# include <functional>
# include <memory>
# include <mutex>
# include <queue>
# include <thread>
# include <variant>
# include <vector>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

extern int get_num_capabilities();

template<typename T>
struct async;

struct thread_pool {
	thread_pool() = delete;
	thread_pool(const thread_pool&) = delete;
	thread_pool(thread_pool&&) = delete;
	thread_pool& operator=(const thread_pool&) = delete;
	thread_pool& operator=(thread_pool&&) = delete;

	explicit thread_pool(int aThreadcMax);
	~thread_pool();

private:
	template<typename T> friend struct async;

	using task = std::function<void(thread_pool&)>;

	void need_thread() noexcept;
	void thread_loop() noexcept;

	size_t                    threadc_max;
	size_t                    threadc; /* threads.size() <= threadc */
	std::vector<std::thread>  threads;
	std::queue<task>          tasks;
	std::mutex                mutex;
	std::condition_variable   cond_on_tasks_and_cancel;
	std::condition_variable   cond_on_tasks_and_task_state;
	bool                      cancel;
};

template<typename T>
struct async {
	async() = delete;
	async(thread_pool&, const std::function<T(void)>&);
	T wait(thread_pool&);

private:
	struct nothing {};
	using state = std::variant<nothing, T, std::exception_ptr>;
	std::shared_ptr<state> result;
};

struct unit {};

template<typename T>
async<T>::async(thread_pool& aPool, const std::function<T(void)>& anAction)
	: result(std::make_shared<state>(nothing{}))
{
	struct task_closure {
		void operator()(thread_pool& aPool)
		{
			assert(result);
			assert(std::holds_alternative<nothing>(*result));
			try {
				T res = action();
				{
					std::unique_lock guard(aPool.mutex);
					*result = std::move(res);
				}
			}
			catch (...) {
				std::exception_ptr eptr = std::current_exception();
				{
					std::unique_lock guard(aPool.mutex);
					*result = eptr;
				}
			}
			aPool.cond_on_tasks_and_task_state.notify_all();
		}

		std::function<T(void)> action;
		std::shared_ptr<state> result;
	};

	task_closure task = {
		.action = anAction,
		.result = result
	};

	{
		std::unique_lock guard(aPool.mutex);
		aPool.tasks.push(task);
	}
	aPool.cond_on_tasks_and_cancel.notify_one();
	aPool.cond_on_tasks_and_task_state.notify_one();
	aPool.need_thread();
}

template<typename T>
T async<T>::wait(thread_pool& aPool)
{
	assert(result);

	for (;;) {
		thread_pool::task task;
		{
			std::unique_lock guard(aPool.mutex);

			aPool.cond_on_tasks_and_task_state.wait(guard, [this, &aPool] {
				return std::holds_alternative<T>(*result)
					|| std::holds_alternative<std::exception_ptr>(*result)
					|| !aPool.tasks.empty();
			});

			if (std::holds_alternative<T>(*result)
				|| std::holds_alternative<std::exception_ptr>(*result))
			{
				break;
				/* NOTREACHED */
			}

			assert(!aPool.tasks.empty());
			task = aPool.tasks.front();
			aPool.tasks.pop();
		}
		task(aPool);
		task = thread_pool::task();
	}

	if (std::holds_alternative<T>(*result)) {
		return std::get<T>(*result);
		/* NOTREACHED */
	}

	assert(std::holds_alternative<std::exception_ptr>(*result));
	std::exception_ptr eptr = std::get<std::exception_ptr>(*result);
	assert(eptr);
	std::rethrow_exception(eptr);
	/* NOTREACHED */
}

}

#endif /* !defined(__BB_GENERAL_ASYNC_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

