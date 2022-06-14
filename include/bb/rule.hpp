
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULE_HPP__
# define __BB_RULE_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>
# include <functional>
# include <memory>
# include <vector>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

template<typename T>
struct object_cls {
	const std::type_info&                   type_info = typeid(T);
	std::function<std::string(const T&)>    show;
	std::function<std::size_t(const T&)>    hash;
	std::function<bool(const T&, const T&)> equal;
};

template<typename Key, typename Value>
struct rule_cls {
	object_cls<Key>   key_cls;
	object_cls<Value> value_cls;
};

template<typename Key, typename Value>
void
add_rule(
	rules& Rules,
	int aPrio,
	const std::function<bool(const Key&)>& aPred,
	const std::function<Value(acontext&, const Key&)>& anAction
	);

template<typename Key, typename Value>
std::vector<Value> apply_rules(acontext&, const std::vector<Key>&);

template<typename Key, typename Value>
void apply_rules_(acontext&, const std::vector<Key>&);

template<typename Key, typename Value>
Value apply_rule(acontext&, const Key&);

template<typename Key, typename Value>
void apply_rule_(acontext&, const Key&);

/*******************************************************************************
 * %% BeginSection: definitions
 */

using untyped_object     = std::shared_ptr<void>;
using untyped_key        = untyped_object;
using untyped_value      = untyped_object;
using untyped_object_cls = object_cls<untyped_object>;
using untyped_rule_cls   = rule_cls<untyped_key, untyped_value>;

extern void
unsafe_add_rule(
	rules&,
	const untyped_rule_cls&,
	int,
	const std::function<bool(const untyped_key&)>&,
	const std::function<untyped_value(acontext&, const untyped_key&)>&
	);

extern std::vector<untyped_value>
unsafe_apply_rules(
	acontext&,
	const untyped_rule_cls&,
	const std::vector<untyped_key>&
	);

extern void
unsafe_apply_rules_(
	acontext&,
	const untyped_rule_cls&,
	const std::vector<untyped_key>&
	);

extern untyped_value
unsafe_apply_rule(
	acontext&,
	const untyped_rule_cls&,
	const untyped_key&
	);

extern void
unsafe_apply_rule_(
	acontext&,
	const untyped_rule_cls&,
	const untyped_key&
	);

template<typename Key, typename Value>
void
add_rule(
	rules& Rules,
	int aPrio,
	const std::function<bool(const Key&)>& aPred,
	const std::function<Value(acontext&, const Key&)>& anAction
	)
{
	const untyped_rule_cls& cls = untyped_rule_cls_instance(
		static_cast<const Key *>(nullptr),
		static_cast<const Value *>(nullptr));

	unsafe_add_rule(Rules, cls, aPrio,
		[aPred](const untyped_key& aKey) {
			return aPred(*std::static_pointer_cast<Key, void>(aKey));
		},
		[anAction](acontext& aCtx, const untyped_key& aKey) {
			return std::static_pointer_cast<void, Value>(
				std::make_shared<Value>(anAction(aCtx,
					*std::static_pointer_cast<Key, void>(aKey))));
		});
}

template<typename Key, typename Value>
std::vector<Value> apply_rules(acontext& aCtx, const std::vector<Key>& Keys)
{
	const untyped_rule_cls& cls = untyped_rule_cls_instance(
		static_cast<const Key *>(nullptr),
		static_cast<const Value *>(nullptr));

	std::vector<untyped_key> untypedKeys;
	for (const Key& k : Keys) {
		untypedKeys.push_back(
			std::static_pointer_cast<void, Key>(std::make_shared<Key>(k)));
	}

	std::vector<untyped_value> untypedValues =
		unsafe_apply_rules(aCtx, cls, untypedKeys);

	std::vector<Value> values;
	for (const untyped_value& untypedValue : untypedValues)
		values.push_back(*std::static_pointer_cast<Value, void>(untypedValue));
	return values;
}

template<typename Key, typename Value>
void apply_rules_(acontext& aCtx, const std::vector<Key>& Keys)
{
	const untyped_rule_cls& cls = untyped_rule_cls_instance(
		static_cast<const Key *>(nullptr),
		static_cast<const Value *>(nullptr));

	std::vector<untyped_key> untypedKeys;
	for (const Key& k : Keys) {
		untypedKeys.push_back(
			std::static_pointer_cast<void, Key>(std::make_shared<Key>(k)));
	}
	unsafe_apply_rules_(aCtx, cls, untypedKeys);
}

template<typename Key, typename Value>
Value apply_rule(acontext& aCtx, const Key& aKey)
{
	const untyped_rule_cls& cls = untyped_rule_cls_instance(
		static_cast<const Key *>(nullptr),
		static_cast<const Value *>(nullptr));

	untyped_key untypedKey =
		std::static_pointer_cast<void, Key>(std::make_shared<Key>(aKey));
	untyped_value untypedValue = unsafe_apply_rule(aCtx, cls, untypedKey);
	return *std::static_pointer_cast<Value, void>(untypedValue);
}

template<typename Key, typename Value>
void apply_rule_(acontext& aCtx, const Key& aKey)
{
	const untyped_rule_cls& cls = untyped_rule_cls_instance(
		static_cast<const Key *>(nullptr),
		static_cast<const Value *>(nullptr));

	untyped_key untypedKey =
		std::static_pointer_cast<void, Key>(std::make_shared<Key>(aKey));
	unsafe_apply_rule_(aCtx, cls, untypedKey);
}

/*******************************************************************************
 * %% BeginSection: helpers
 */

template<typename T>
untyped_object_cls to_untyped_object_cls(const object_cls<T>& aCls)
{
	return untyped_object_cls {
		.type_info = aCls.type_info,
		.show = [aCls](const untyped_object& anObj) {
			return aCls.show(*std::static_pointer_cast<T, void>(anObj));
		},
		.hash = [aCls](const untyped_object& anObj) {
			return aCls.hash(*std::static_pointer_cast<T, void>(anObj));
		},
		.equal = [aCls](const untyped_object& aLhs, const untyped_object& aRhs) {
			return aCls.equal(*std::static_pointer_cast<T, void>(aLhs),
				*std::static_pointer_cast<T, void>(aRhs));
		}
	};
}

template<typename Key, typename Value>
const untyped_rule_cls& untyped_rule_cls_instance(const Key* k, const Value* v)
{
	const rule_cls<Key, Value>& ruleCls = rule_cls_instance(k, v);
	static const untyped_object_cls untypedKeyCls =
		to_untyped_object_cls(ruleCls.key_cls);
	static const untyped_object_cls untypedValueCls =
		to_untyped_object_cls(ruleCls.value_cls);
	static const untyped_rule_cls untypedRuleCls = {
		.key_cls = untypedKeyCls,
		.value_cls = untypedValueCls
	};
	return untypedRuleCls;
}

}

#endif /* !defined(__BB_RULE_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

