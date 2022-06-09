
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
unsafe_add_rule(
	rules& Rules,
	const untyped_rule_cls& aCls,
	int aPrio,
	const std::function<bool(const untyped_key&)>& aPred,
	const std::function<untyped_value(acontext&, const untyped_key&)>& anAction
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
	const untyped_rule_cls& aCls,
	const untyped_key& aKey
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

untyped_value
unsafe_apply_rule(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const untyped_key& aKey
	)
{
	untyped_value retval;

	Key ruleKey(aCls.key_cls, aKey);
	if (aCtx.inprogress_keys.count(ruleKey)) {
		throw error_rule_recursion(aCls.key_cls.type_info,
			aCls.key_cls.show(aKey));
		/* NOTREACHED */
	}

	{
		struct key_building_scope {
			acontext&  ctx;
			const Key& key;

			key_building_scope(acontext& aCtx, const Key& aKey)
				: ctx(aCtx), key(aKey)
			{
				aCtx.inprogress_keys.insert(aKey);
			}

			~key_building_scope()
			{
				size_t nerases = ctx.inprogress_keys.erase(key);
				assert(1 == nerases);
				(void) nerases;
			}
		};

		key_building_scope scope(aCtx, ruleKey);
		const rule& r = _t_lookup_rule(aCtx, aCls, aKey);
		retval = r.action(aCtx, aKey);
	}

	return (retval);
}

void
unsafe_apply_rule_(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const untyped_key& aKey
	)
{
	unsafe_apply_rule(aCtx, aCls, aKey);
}

std::vector<untyped_value>
unsafe_apply_rules(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const std::vector<untyped_key>& Keys
	)
{
	std::vector<untyped_value> values;
	// @todo: redesign with staunch mode
	for (const untyped_key& k : Keys)
		values.push_back(unsafe_apply_rule(aCtx, aCls, k));
	return (values);
}

void
unsafe_apply_rules_(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const std::vector<untyped_key>& Keys
	)
{
	for (const untyped_key& k : Keys)
		unsafe_apply_rule_(aCtx, aCls, k);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

