
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/parallelI.hpp"
#include "bb/run.hpp"
#include "bb/rules/file.hpp"
#include <mutex>
#include <algorithm>

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

static void _t_default_rules(rules& Rules)
{
	default_file_rule(Rules);
}

static void _t_sort_rules(rules& Rules)
{
	for (auto& r : Rules.crules) {
		std::sort(r.second.prules.begin(), r.second.prules.end(),
			[](const prule& x, const prule& y) {
				return (x.priority > y.priority);
			});
	}
}

static int
_t_run(const options& Options, const std::function<void(rules&)>& aRulesGen)
{
	rules rules;

	try {
		_t_default_rules(rules);
		aRulesGen(rules);
	}
	catch (const std::exception& err) {
		Options.output(ERROR, err.what());
		return 1;
		/* NOTREACHED */
	}
	catch (...) {
		Options.output(ERROR, "Error: caught unknown type of exception while"
			" generating build rules");
		return 1;
		/* NOTREACHED */
	}

	if (rules.actions.empty()) {
		Options.output(INFO,
			"Warning: no want/action statements, nothing to do");
		return 0;
		/* NOTREACHED */
	}

	_t_sort_rules(rules);

	benv buildEnv(Options, rules);
	acontext ctx(buildEnv);
	int nerrors = 0;
	try {
		sequenceP_<void>(ctx, rules.actions);
	}
	catch (const std::exception& err) {
		Options.output(ERROR, err.what());
		++nerrors;
	}
	catch (...) {
		Options.output(ERROR,
			"Error: caught unknown type of exception while building");
		++nerrors;
	}

	return nerrors;
}

int run(const options& Options, const std::function<void(rules&)>& aRulesGen)
{
	options canon = Options;
	std::mutex outputMutex;

	canon.nthreads = (Options.nthreads <= 0 ? get_num_capabilities()
	                                        : Options.nthreads);
	canon.output = [&Options, &outputMutex]
		(verbosity aVerbosity, const std::string& aMsg)
		{
			if (aVerbosity <= Options.verbosity) {
				std::unique_lock lock(outputMutex);
				Options.output(aVerbosity, aMsg);
			}
		};
	return _t_run(canon, aRulesGen);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

