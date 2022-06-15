
/* -------------------------------------------------------------------------- */

#ifndef __BB_PARALLEL_I_HPP__
# define __BB_PARALLEL_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/typesI.hpp"
# include "bb/log.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

struct exception_handler {
	exception_handler() : prev_err() {}

	inline void operator() (acontext& aCtx)
	{
		std::exception_ptr currErr = std::current_exception();
		if (!aCtx.benv.options.staunch) {
			std::rethrow_exception(currErr);
			/* NOTREACHED */
		}

		if (prev_err) {
			try {
				std::rethrow_exception(prev_err);
			}
			catch (const std::exception& err) {
				error(aCtx, err.what());
			}
		}
		prev_err = currErr;
	}

	inline void finalize()
	{
		if (prev_err) {
			std::rethrow_exception(prev_err);
			/* NOTREACHED */
		}
	}

private:
	std::exception_ptr  prev_err;
};


template<typename A>
void
sequenceS_(
	acontext& aCtx,
	const std::vector< std::function<A(acontext&)> >& Actions
	)
{
	exception_handler handler;

	for (const auto& action : Actions) {
		try {
			action(aCtx);
		}
		catch (...) {
			handler(aCtx);
		}
	}
	handler.finalize();
}

template<typename A>
void
sequenceP_(
	acontext& aCtx,
	const std::vector< std::function<A(acontext&)> >& Actions
	)
{
	exception_handler handler;
	std::vector< async<unit> > jobs;

	if (1 == aCtx.benv.options.nthreads || 1 == Actions.size()) {
		sequenceS_(aCtx, Actions);
		return;
		/* NOTREACHED */
	}

	jobs.reserve(Actions.size());
	try {
		for (const auto& action : Actions) {
			jobs.push_back(async<unit>(aCtx.benv.pool,
				[aCtx, action] () mutable
				{
					action(aCtx);
					return unit{};
				}));
		}
	}
	catch (...) {
		assert(0 && "unexpected exception");
	}

	for (auto& job : jobs) {
		try {
			job.wait();
		}
		catch (...) {
			handler(aCtx);
		}
	}
	handler.finalize();
	assert(jobs.size() == Actions.size());
}

template<typename A, typename B>
std::vector<B>
forS(
	acontext& aCtx,
	const std::vector<A>& Inputs,
	const std::function<B(acontext&, const A&)>& aMap
	)
{
	exception_handler handler;
	std::vector<B> outputs;

	outputs.reserve(Inputs.size());
	for (const A& input : Inputs) {
		try {
			outputs.push_back(aMap(aCtx, input));
		}
		catch (...) {
			handler(aCtx);
		}
	}
	handler.finalize();

	assert(outputs.size() == Inputs.size());
	return (outputs);
}

template<typename A, typename B>
std::vector<B>
forP(
	acontext& aCtx,
	const std::vector<A>& Inputs,
	const std::function<B(acontext&, const A&)>& aMap
	)
{
	exception_handler handler;
	std::vector< async<B> > jobs;
	std::vector<B> outputs;

	if (1 == aCtx.benv.options.nthreads || 1 == Inputs.size()) {
		return forS(aCtx, Inputs, aMap);
		/* NOTREACHED */
	}

	jobs.reserve(Inputs.size());
	outputs.reserve(Inputs.size());
	try {
		for (const A& input : Inputs) {
			jobs.push_back(async<B>(aCtx.benv.pool,
				[aCtx, input, aMap] () mutable
				{
					return aMap(aCtx, input);
				}));
		}
	}
	catch (...) {
		assert(0 && "unexpected exception");
	}

	for (auto& job : jobs) {
		try {
			outputs.push_back(job.wait());
		}
		catch (...) {
			handler(aCtx);
		}
	}
	handler.finalize();

	assert(outputs.size() == Inputs.size());
	return (outputs);
}

template<typename A>
void
forS_(
	acontext& aCtx,
	const std::vector<A>& Inputs,
	const std::function<void(acontext&, const A&)>& aMap
	)
{
	exception_handler handler;

	for (const A& input : Inputs) {
		try {
			aMap(aCtx, input);
		}
		catch (...) {
			handler(aCtx);
		}
	}
	handler.finalize();
}

template<typename A>
void
forP_(
	acontext& aCtx,
	const std::vector<A>& Inputs,
	const std::function<void(acontext&, const A&)>& aMap
	)
{
	exception_handler handler;
	std::vector< async<unit> > jobs;

	if (1 == aCtx.benv.options.nthreads || 1 == Inputs.size()) {
		forS_(aCtx, Inputs, aMap);
		return;
		/* NOTREACHED */
	}

	jobs.reserve(Inputs.size());
	try {
		for (const A& input : Inputs) {
			jobs.push_back(async<unit>(aCtx.benv.pool,
				[aCtx, input, aMap] () mutable
				{
					aMap(aCtx, input);
					return unit{};
				}));
		}
	}
	catch (...) {
		assert(0 && "unexpected exception");
	}

	for (auto& job : jobs) {
		try {
			job.wait();
		}
		catch (...) {
			handler(aCtx);
		}
	}
	handler.finalize();
}

}

#endif /* !defined(__BB_PARALLEL_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

