
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULE_I_HPP__
# define __BB_RULE_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/rule.hpp"
# include <vector>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb {

struct rule {
	rule(
		const std::function<bool(const untyped_key&)>& aPred,
		const std::function<untyped_value(acontext&, const untyped_key&)>& anAction
		)
		: predicate(aPred), action(anAction)
	{}

	std::function<bool(const untyped_key&)>                     predicate;
	std::function<untyped_value(acontext&, const untyped_key&)> action;
};

struct prule {
	prule(int aPrio, const rule& aRule)
		: priority(aPrio), rule(aRule)
	{}

	int   priority;
	rule  rule;
};

struct crule {
	crule(const untyped_rule_cls& aCls, const std::vector<prule>& PRules)
		: cls(aCls), prules(PRules)
	{}

	const untyped_rule_cls& cls;
	std::vector<prule>      prules;
};

}

#endif /* !defined(__BB_RULE_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

