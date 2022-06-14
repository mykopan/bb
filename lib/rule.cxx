
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
	auto iter = aCtx.benv.rules.crules.find(aCls.key_cls.type_info);
	if (aCtx.benv.rules.crules.end() == iter) {
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
	struct key_building_scope {
		acontext&               actx;
		dynamic_key             key;

		key_building_scope(
			acontext& aCtx,
			const untyped_rule_cls& aCls,
			const untyped_key& aKey
			)
			: actx(aCtx), key(aCls.key_cls, aKey)
		{
			bool recursion;
			{
				std::unique_lock lock(actx.benv.inprogress_keys_mutex);
				if (!(recursion = !!actx.benv.inprogress_keys.count(key)))
					actx.benv.inprogress_keys.insert(key);
			}
			if (recursion) {
				throw error_rule_recursion(key.type_info(), key.show());
				/* NOTREACHED */
			}

			actx.stack.push_back(key);
		}

		~key_building_scope()
		{
			assert(!actx.stack.empty());
			actx.stack.pop_back();

			size_t nerases;
			{
				std::unique_lock lock(actx.benv.inprogress_keys_mutex);
				nerases = actx.benv.inprogress_keys.erase(key);
			}
			assert(1 == nerases);
			(void) nerases;
		}
	};
	untyped_value retval;

	key_building_scope scope(aCtx, aCls, aKey);
	try {
		const rule& r = _t_lookup_rule(aCtx, aCls, aKey);
		retval = r.action(aCtx, aKey);
	}
	catch (const bb_exception& err) {
		throw;
		/* NOTREACHED */
	}
	catch (const std::exception& err) {
		std::vector<std::string> keys;
		for (const auto& k : aCtx.stack)
			keys.push_back(k.show());
		throw bb_exception(keys, err);
		/* NOTREACHED */
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

template<typename A, typename B>
std::vector<B>
forP(
	acontext& aCtx,
	const std::vector<A>& Inputs,
	const std::function<B(acontext&, const A&)>& aMap
	)
{
	std::vector<B> outputs;
	// @todo: redesign with staunch mode
	for (const A& input : Inputs)
		outputs.push_back(aMap(aCtx, input));
	return (outputs);
}

template<typename A>
void
forP_(
	acontext& aCtx,
	const std::vector<A>& Inputs,
	const std::function<void(acontext&, const A&)>& aMap
	)
{
	// @todo: redesign with staunch mode
	for (const A& input : Inputs)
		aMap(aCtx, input);
}

std::vector<untyped_value>
unsafe_apply_rules(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const std::vector<untyped_key>& Keys
	)
{
	return forP<untyped_key, untyped_value>(aCtx, Keys,
		[aCls](acontext& aCtx, const untyped_key& aKey) {
			return unsafe_apply_rule(aCtx, aCls, aKey);
		});
}

void
unsafe_apply_rules_(
	acontext& aCtx,
	const untyped_rule_cls& aCls,
	const std::vector<untyped_key>& Keys
	)
{
	return forP_<untyped_key>(aCtx, Keys,
		[aCls](acontext& aCtx, const untyped_key& aKey) {
			unsafe_apply_rule_(aCtx, aCls, aKey);
		});
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

