
/* -------------------------------------------------------------------------- */

#ifndef __BB_TYPES_I_HPP__
# define __BB_TYPES_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/types.hpp"
# include "bb/dynobjI.hpp"
# include "bb/ruleI.hpp"
# include "bb/general/asyncI.hpp"
# include <typeindex>
# include <mutex>
# include <unordered_map>
# include <unordered_set>
# include <vector>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb {

struct rules {
	std::vector< std::function<void(acontext&)> > actions;
	std::unordered_map<std::type_index, crule>    crules;
};

struct benv {
	benv(const options& Options, const rules& Rules)
		: options(Options)
		, rules(Rules)
		, inprogress_keys()
		, pool(Options.nthreads - 1)
	{}

	const options&                    options;
	const rules&                      rules;
	std::unordered_set<dynamic_key>   inprogress_keys;
	std::mutex                        inprogress_keys_mutex;
	thread_pool                       pool;
};

struct acontext {
	explicit acontext(benv& anEnv)
		: benv(anEnv), stack()
	{}
	// @todo: stack copying may be overhead,
	// may need to use another stack representation
	acontext(const acontext& aSrc)
		: benv(aSrc.benv), stack(aSrc.stack)
	{}

	benv&                     benv;
	std::vector<dynamic_key>  stack;
};

}

#endif /* !defined(__BB_TYPES_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

