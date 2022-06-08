
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/errorsI.hpp"
#include "bb/rule.hpp"
#include "bb/typesI.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

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
			throw error_incompatible_rules(aCls.key_cls.type_info,
				crule.cls.value_cls.type_info,
				aCls.value_cls.type_info);
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
	const acontext& aCtx,
	const rule_cls& aCls,
	const key& aKey
	)
{
	auto iter = aCtx.rules.crules.find(aCls.key_cls.type_info);
	if (aCtx.rules.crules.end() == iter) {
		throw error_no_rule_to_build_type(aCls.key_cls.type_info,
			aCls.key_cls.show(aKey), aCls.value_cls.type_info);
		/* NOTREACHED */
	}
	const crule& crule = iter->second;

	assert(crule.cls.key_cls.type_info == aCls.key_cls.type_info);
	if (crule.cls.value_cls.type_info != aCls.value_cls.type_info) {
		throw error_rule_type_mismatch(aCls.key_cls.type_info,
			aCls.key_cls.show(aKey),
			crule.cls.value_cls.type_info,
			aCls.value_cls.type_info);
		/* NOTREACHED */
	}

	size_t nmatches = 0;
	auto pruleIter = crule.prules.begin();
	for (; crule.prules.end() != pruleIter; ++pruleIter) {
		if (pruleIter->rule.predicate(aKey)) {
			++nmatches;
			/* check that there are not others rules with same priority and
			 * which satisfies predicate */
			// @todo: maybe this check should be optional?
			for (auto pruleIter2 = std::next(pruleIter);
					crule.prules.end() != pruleIter2
					&& pruleIter->priority == pruleIter2->priority;
						++pruleIter2)
			{
				if (pruleIter2->rule.predicate(aKey))
					++nmatches;
			}
			break;
			/* NOTREACHED */
		}
	}

	if (1 != nmatches) {
		throw error_multiple_rules_match(aCls.key_cls.type_info,
			aCls.key_cls.show(aKey), nmatches);
		/* NOTREACHED */
	}

	return (pruleIter->rule);
}

value apply_rule(acontext& aCtx, const rule_cls& aCls, const key& aKey)
{
	const rule& r = _t_lookup_rule(aCtx, aCls, aKey);
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

