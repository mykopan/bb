
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/rules.hpp"
#include "bb/errorsI.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

static rules& _t_merge_rules(rules& aSrcDst, const rules& aSrc)
{
	append(aSrcDst.actions, aSrc.actions);

	for (const auto& tcruleSrc : aSrc.crules) {
		const auto& typeOfKey = tcruleSrc.first;
		const auto& cruleSrc = tcruleSrc.second;
		auto iter = aSrcDst.crules.find(typeOfKey);
		if (aSrcDst.crules.end() != iter) {
			auto& cruleSrcDst = iter->second;

			assert(cruleSrcDst.cls.key_cls.type_info == cruleSrc.cls.key_cls.type_info);

			if (cruleSrcDst.cls.value_cls.type_info != cruleSrc.cls.value_cls.type_info)
			{
				throw error_incompatible_rules(
					cruleSrcDst.cls.key_cls.type_info.name(),
					cruleSrcDst.cls.value_cls.type_info.name(),
					cruleSrc.cls.value_cls.type_info.name());
				/* NOTREACHED */
			}
			append(cruleSrcDst.prules, cruleSrc.prules);
		}
		else {
			aSrcDst.crules.insert({ typeOfKey, cruleSrc });
		}
	}
	return (aSrcDst);
}

void
priority(
	rules& Rules,
	int aPrio,
	const std::function<void(rules&)>& aRulesGen
	)
{
	rules rules;

	aRulesGen(rules);
	for (auto& tcrule : rules.crules) {
		for (auto& prule : tcrule.second.prules)
			prule.priority = aPrio;
	}

	_t_merge_rules(Rules, rules);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

