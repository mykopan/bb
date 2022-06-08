
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.h"
#include "bb/rule.h"

#include <sstream>
#include <optional>

/*******************************************************************************
 * %% BeginSection: function definitions
 */
# pragma mark -
# pragma mark [ function definitions ]
# pragma mark -

namespace bb {

void
add_rule(
	rules& Rules,
	const rule_cls& aCls,
	int aPrio,
	const std::function<bool(const key&)>& aPred,
	const std::function<value(acontext&, const key&)>& anAction
	)
{
	auto iter = Rules.crules.find(aCls.key_cls.type_info);
	if (Rules.crules.end() != iter) {
		auto& crule = iter->second;

		assert(crule.cls.key_cls.type_info == aCls.key_cls.type_info);

		if (crule.cls.value_cls.type_info != aCls.value_cls.type_info) {
			std::stringstream errmsg;
			errmsg << "Error: cannot add rule of type '" <<
				aCls.key_cls.type_info.name() << " -> " <<
				aCls.value_cls.type_info.name() <<
				"' because has rule of type '" <<
				crule.cls.key_cls.type_info.name() << " -> " <<
				crule.cls.value_cls.type_info.name() <<
				"' (rule value type cannot be different for the same type of key)";
			throw std::runtime_error(errmsg.str());
			/* NOTREACHED */
		}
		crule.prules.push_back(prule(aPrio, rule(aPred, anAction)));
	}
	else {
		Rules.crules.insert({ aCls.key_cls.type_info,
			crule(aCls, { prule(aPrio, rule(aPred, anAction)) })});
	}
}

static const rule&
_t_lookup_rule(
	char const *aFuncName,
	const acontext& aCtx,
	const rule_cls& aCls,
	const key& aKey
	)
{
	auto iter = aCtx.rules.crules.find(aCls.key_cls.type_info);
	if (aCtx.rules.crules.end() == iter) {
		std::stringstream errmsg;
		errmsg << "Error: " << aFuncName << ": not found rule of type '" <<
			aCls.key_cls.type_info.name() << " -> " <<
			aCls.value_cls.type_info.name() <<
			"' for build " << aCls.key_cls.show(aKey);
		throw std::runtime_error(errmsg.str());
		/* NOTREACHED */
	}
	const crule& crule = iter->second;

	assert(crule.cls.key_cls.type_info == aCls.key_cls.type_info);
	if (crule.cls.value_cls.type_info != aCls.value_cls.type_info) {
		std::stringstream errmsg;
		errmsg << "Error: " << aFuncName << ": cannot found rule of type '" <<
			aCls.key_cls.type_info.name() << " -> " <<
			aCls.value_cls.type_info.name() <<
			"' for build " << aCls.key_cls.show(aKey) <<
			", but has rule of type '" <<
			crule.cls.key_cls.type_info.name() << " -> " <<
			crule.cls.value_cls.type_info.name() <<
			"' (rule value type cannot be different for the same type of key)";
		throw std::runtime_error(errmsg.str());
		/* NOTREACHED */
	}

	for (auto pruleIter = crule.prules.begin();
			crule.prules.end() != pruleIter;
				++pruleIter)
	{
		if (pruleIter->rule.predicate(aKey)) {
			/* check that there are not others rules with same priority and
			 * which satisfies predicate */
			// @todo: maybe this check should be optional?
			for (auto pruleIter2 = std::next(pruleIter);
					crule.prules.end() != pruleIter2
					&& pruleIter->priority == pruleIter2->priority;
						++pruleIter2)
			{
				if (pruleIter2->rule.predicate(aKey)) {
					std::stringstream errmsg;
					errmsg << "Error: " << aFuncName <<
						": found at least two build rules of type '" <<
						aCls.key_cls.type_info.name() << " -> " <<
						aCls.value_cls.type_info.name() <<
						"' with the same priority that satisfy the predicate"
						" on the key " << aCls.key_cls.show(aKey) <<
						" (all rules with the same priority must be disjoint)";
					throw std::runtime_error(errmsg.str());
					/* NOTREACHED */
				}
			}

			return (pruleIter->rule);
			/* NOTREACHED */
		}
	}

	std::stringstream errmsg;
	errmsg << "Error: " << aFuncName << ": no rule available for build: " <<
		aCls.key_cls.show(aKey);
	throw std::runtime_error(errmsg.str());
}

value apply_rule(acontext& aCtx, const rule_cls& aCls, const key& aKey)
{
	const rule& r = _t_lookup_rule("bb::apply_rule", aCtx, aCls, aKey);
	return r.action(aCtx, aKey);
}

void apply_rule_(acontext& aCtx, const rule_cls& aCls, const key& aKey)
{
	apply_rule(aCtx, aCls, aKey);
}

std::vector<value>
apply_rules(
	acontext& aCtx,
	const rule_cls& aCls,
	const std::vector<key>& Keys
	)
{
	std::vector<value> values;
	// @todo: redesign with staunch mode
	for (const key& k : Keys)
		values.push_back(apply_rule(aCtx, aCls, k));
	return (values);
}

void
apply_rules_(
	acontext& aCtx,
	const rule_cls& aCls,
	const std::vector<key>& Keys
	)
{
	for (const key& k : Keys)
		apply_rule_(aCtx, aCls, k);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

