
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/rules.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */
# pragma mark -
# pragma mark [ function definitions ]
# pragma mark -

namespace bb {

template<typename T>
static std::vector<T>&
_t_vector_append(
	std::vector<T>& aSrcDst,
	const std::vector<T>& aSrc
	)
{
	aSrcDst.insert(aSrcDst.end(), aSrc.begin(), aSrc.end());
	return (aSrcDst);
}

static rules& _t_merge_rules(rules& aSrcDst, const rules& aSrc)
{
	_t_vector_append(aSrcDst.actions, aSrc.actions);

	for (const auto& typrulSrc : aSrc.rules) {
		std::type_index keyTypeIndex = typrulSrc.first;
		const auto& rulesSrc = typrulSrc.second;

		auto iter = aSrcDst.rules.find(keyTypeIndex);
		if (aSrcDst.rules.end() != iter) {
			auto rulesSrcDst = iter->second;
			const auto& valueTypeInfo = rulesSrc.first.value_cls.type_info;
			const auto& value2TypeInfo = rulesSrcDst.first.value_cls.type_info;

			assert(rulesSrcDst.first.key_cls.type_info == rulesSrc.first.key_cls.type_info);
			if (value2TypeInfo != valueTypeInfo) {
				throw std::runtime_error(std::string("cannot add rule of type '")
					+ keyTypeIndex.name() + " -> " + valueTypeInfo.name()
					+ "' because has rule of type '"
					+ keyTypeIndex.name() + " -> " + value2TypeInfo.name()
					+ "' (rule value type cannot be different for the same type of key)"
					);
			}
			_t_vector_append(rulesSrcDst.second, rulesSrc.second);
		}
		else {
			aSrcDst.rules.insert({ keyTypeIndex, rulesSrc });
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
	for (auto& x : rules.rules) {
		for (auto& y : x.second.second)
			y.first = aPrio;
	}

	_t_merge_rules(Rules, rules);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

