
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/general/asyncI.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

int get_num_capabilities()
{
	return std::max(1U, std::thread::hardware_concurrency());
}

thread_pool::thread_pool(int aThreadcMax)
	: threadc_max(std::max(0, aThreadcMax))
	, threadc(0)
	, threads()
	, tasks()
	, mutex()
	, cond_on_tasks_and_cancel()
	, cond_on_tasks_and_task_state()
	, cancel(false)
{
}

thread_pool::~thread_pool()
{
	{
		std::unique_lock guard(mutex);
		cancel = true;
	}
	cond_on_tasks_and_cancel.notify_all();

	for (std::thread& thread : threads)
		thread.join();
}

void thread_pool::need_thread() noexcept
{
	bool need;
	{
		std::unique_lock guard(mutex);
		need = threadc < threadc_max
			&& threadc < tasks.size();
		if (need)
			++threadc;
	}
	if (!need) {
		return;
		/* NOTREACHED */
	}

	try {
		std::thread thread = std::thread(&thread_pool::thread_loop, this);
		{
			std::unique_lock guard(mutex);
			threads.push_back(std::move(thread));
		}
	}
	catch (...) {
		std::unique_lock guard(mutex);
		--threadc;
	}
}

void thread_pool::thread_loop() noexcept
{
	std::function<void(void)> task;
	for (;;) {
		{
			std::unique_lock guard(mutex);

			cond_on_tasks_and_cancel.wait(guard, [this] {
				return !tasks.empty() || cancel;
			});
			if (cancel) {
				break;
				/* NOTREACHED */
			}

			assert(!tasks.empty());
			task = tasks.front();
			tasks.pop();
		}
		task();
		task = std::function<void(void)>();
	}
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

