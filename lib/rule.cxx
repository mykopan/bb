
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
			throw error_incompatible_rules(aCls.key_cls.type_info.name(),
				crule.cls.value_cls.type_info.name(),
				aCls.value_cls.type_info.name());
			/* NOTREACHED */
		}
		crule.prules.push_back(prule(aPrio, rule(aPred, anAction)));
	}
	else {
		Rules.crules.insert({ aCls.key_cls.type_info,
			crule(aCls, { prule(aPrio, rule(aPred, anAction)) })});
	}
}

static const crule&
_t_lookup_crule(
	const std::unordered_map<std::type_index, crule>& Rules,
	const dynamic_key& aKey
	)
{
	auto iter = Rules.find(aKey.type_info());
	if (Rules.end() == iter) {
		throw error_no_rule_to_build_type(aKey.type_info().name(), aKey.show(),
			std::nullopt);
		/* NOTREACHED */
	}
	return iter->second;
}

static std::pair<const untyped_rule_cls&, const rule&>
_t_lookup_rule(
	const std::unordered_map<std::type_index, crule>& Rules,
	const dynamic_key& aKey
	)
{
	const crule& crule = _t_lookup_crule(Rules, aKey);

	size_t nmatches = 0;
	auto pruleIter = crule.prules.begin();
	for (; crule.prules.end() != pruleIter; ++pruleIter) {
		if (pruleIter->rule.predicate(aKey.obj)) {
			++nmatches;
			/* check that there are not others rules with same priority and
			 * which satisfies predicate */
			// @todo: maybe this check should be optional?
			for (auto pruleIter2 = std::next(pruleIter);
					crule.prules.end() != pruleIter2
					&& pruleIter->priority == pruleIter2->priority;
						++pruleIter2)
			{
				if (pruleIter2->rule.predicate(aKey.obj))
					++nmatches;
			}
			break;
			/* NOTREACHED */
		}
	}

	if (1 != nmatches) {
		throw error_multiple_rules_match(aKey.type_info().name(), aKey.show(),
			nmatches);
		/* NOTREACHED */
	}

	return std::pair<const untyped_rule_cls&, const rule&>(crule.cls, pruleIter->rule);
}

static std::vector<dynamic_value>
_t_apply_rules(acontext& aCtx, const std::vector<dynamic_key>& Keys)
{
	db::build_ops ops = {
		.valid = [&aCtx](const dynamic_key& aKey, const dynamic_value& anOld) {
			const untyped_rule_cls& rcls =
				_t_lookup_crule(aCtx.benv.rules.crules, aKey).cls;
			if (rcls.value_cls.type_info != anOld.type_info()) {
				return false;
				/* NOTREACHED */
			}
			std::optional<untyped_value> mbVal = rcls.stored(aKey.obj);
			return mbVal ? rcls.value_cls.equal(anOld.obj, *mbVal) : false;
		},
		.build = [&aCtx](const db::stack& aStack, const dynamic_key& aKey) {
			auto r = _t_lookup_rule(aCtx.benv.rules.crules, aKey);
			acontext newCtx(aCtx.benv, aStack);
			untyped_value value = r.second.action(newCtx, aKey.obj);
			return std::make_pair(dynamic_value(r.first.value_cls, value),
				newCtx.depends);
		}
	};

	auto res = db::build(aCtx.benv.database, ops, aCtx.stack, Keys);
	append(aCtx.depends, res.second);
	return res.first;
}

std::vector<untyped_value>
unsafe_apply_rules(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const std::vector<untyped_key>& Keys
	)
{
	auto iter = aCtx.benv.rules.crules.find(aCls.key_cls.type_info);
	if (aCtx.benv.rules.crules.end() == iter) {
		throw error_no_rule_to_build_type(aCls.key_cls.type_info.name(),
			Keys.empty() ? std::nullopt
			             : std::make_optional(aCls.key_cls.show(Keys[0])),
			aCls.value_cls.type_info.name());
		/* NOTREACHED */
	}
	const crule& crule = iter->second;

	assert(crule.cls.key_cls.type_info == aCls.key_cls.type_info);
	if (crule.cls.value_cls.type_info != aCls.value_cls.type_info) {
		throw error_rule_type_mismatch(aCls.key_cls.type_info.name(),
			Keys.empty() ? std::nullopt
			             : std::make_optional(aCls.key_cls.show(Keys[0])),
			crule.cls.value_cls.type_info.name(),
			aCls.value_cls.type_info.name());
		/* NOTREACHED */
	}

	std::vector<dynamic_key> dynKeys;
	dynKeys.reserve(Keys.size());
	for (const auto& key : Keys)
		dynKeys.emplace_back(aCls.key_cls, key);

	std::vector<dynamic_value> dynValues = _t_apply_rules(aCtx, dynKeys);

	std::vector<untyped_value> retval;
	retval.reserve(dynValues.size());
	for (auto& dynValue : dynValues) {
		assert(aCls.value_cls.type_info == dynValue.type_info());
		retval.emplace_back(dynValue.obj);
	}
	return retval;
}

void
unsafe_apply_rules_(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const std::vector<untyped_key>& Keys
	)
{
	unsafe_apply_rules(aCtx, aCls, Keys);
}

untyped_value
unsafe_apply_rule(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const untyped_key& aKey
	)
{
	auto values = unsafe_apply_rules(aCtx, aCls, {aKey});
	return values.front();
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

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

