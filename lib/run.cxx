
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
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

int run(const options& Options, const std::function<void(rules&)>& aRulesGen)
{
	int nerrors = 0;
	options canonOptions = Options;
	rules rules;
	std::unordered_set<dynamic_key> inprogressKeys;
	std::mutex outputGuard;

	canonOptions.output =
		[ requiredVerbosity = Options.verbosity, &output = Options.output
		, &outputGuard
		]
		(verbosity aVerbosity, const std::string& aMsg) mutable
		{
			if (aVerbosity <= requiredVerbosity) {
				std::unique_lock lock(outputGuard);
				output(aVerbosity, aMsg);
			}
		};

	try {
		_t_default_rules(rules);
		aRulesGen(rules);
	}
	catch (const std::exception& err) {
		canonOptions.output(ERROR, err.what());
		return 1;
		/* NOTREACHED */
	}
	catch (...) {
		canonOptions.output(ERROR,
			"Error: caught unknown type of exception while generating build rules");
		return 1;
		/* NOTREACHED */
	}

	if (rules.actions.empty()) {
		canonOptions.output(INFO,
			"Warning: no want/action statements, nothing to do");
		return (0);
		/* NOTREACHED */
	}

	_t_sort_rules(rules);

	for (const auto& act : rules.actions) {
		try {
			acontext context(canonOptions, rules, inprogressKeys);
			act(context);
		}
		catch (const std::exception& err) {
			canonOptions.output(ERROR, err.what());
			if (!canonOptions.staunch) {
				return 1;
				/* NOTREACHED */
			}
			++nerrors;
		}
		catch (...) {
			canonOptions.output(ERROR,
				"Error: caught unknown type of exception while building");
			if (!canonOptions.staunch) {
				return 1;
				/* NOTREACHED */
			}
			++nerrors;
		}
	}

	return nerrors;
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

