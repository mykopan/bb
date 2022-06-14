
#include <bb/general/asyncI.hpp>
#include <iostream>
#include <chrono>

static size_t fact(size_t n)
{
	size_t i, acc;
	acc = 1;
	for (i = 1; i < n; ++i)
		acc *= i;
	return acc;
}

void test_async()
{
	using namespace std::chrono_literals;

	bb::thread_pool pool(bb::get_num_capabilities() - 1);

	{
		std::vector< bb::async<size_t> > tasks;
		for (size_t i = 0; i < 20; ++i) {
			tasks.push_back(bb::async<size_t>(pool, [i] {
				if (5 == i) {
					throw std::runtime_error("error for 5!");
				}
				std::this_thread::sleep_for(i * 1ms);
				return fact(i);
			}));
		}

		for (size_t i = 0; i < tasks.size(); ++i) {
			try {
				auto n = tasks[i].wait();
				assert(fact(i) == n);
			}
			catch (const std::exception& err) {
				assert(i == 5 && std::string(err.what()) == "error for 5!");
			}
		}
	}

	{
		bb::async<std::string> job = bb::async<std::string>(pool, [] {
			return "some result";
		});
		for (size_t i = 0; i < 10; ++i) {
			try {
				auto r = job.wait();
				assert(r == "some result");
			}
			catch (const std::exception& err) {
				assert(0 && "unexpected exception");
			}
		}
	}

	{
		bb::async<std::string> job = bb::async<std::string>(pool, [] {
			throw std::runtime_error("some error");
			return "";
		});
		for (size_t i = 0; i < 10; ++i) {
			try {
				auto r = job.wait();
				assert(0 && "unexpected result");
			}
			catch (const std::exception& err) {
				assert(std::string(err.what()) == "some error");
			}
		}
	}
}

