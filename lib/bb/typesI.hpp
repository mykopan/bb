
/* -------------------------------------------------------------------------- */

#ifndef __BB_TYPES_I_HPP__
# define __BB_TYPES_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/types.hpp"
# include "bb/ruleI.hpp"
# include "bb/databaseI.hpp"
# include "bb/general/vectorI.hpp"
# include "bb/general/asyncI.hpp"

# include <typeindex>
# include <mutex>
# include <unordered_map>
# include <unordered_set>

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
		, database()
		, inprogress_keys()
		, pool(Options.nthreads - 1)
	{}

	const options&                    options;
	const rules&                      rules;
	db::database                      database;
	std::unordered_set<dynamic_key>   inprogress_keys;
	std::mutex                        inprogress_keys_mutex;
	thread_pool                       pool;
};

struct acontext {
	explicit acontext(benv& anEnv)
		: benv(anEnv), stack(), depends()
	{}
	explicit acontext(benv& anEnv, const db::stack& aStack)
		: benv(anEnv), stack(aStack), depends()
	{}
	acontext(const acontext& aSrc)
		: benv(aSrc.benv), stack(aSrc.stack), depends(aSrc.depends)
	{}

	benv&           benv;
	const db::stack stack;
	db::depends     depends;
};

}

#endif /* !defined(__BB_TYPES_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

