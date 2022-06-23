
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULE_HPP__
# define __BB_RULE_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>
# include <bb/binary.hpp>

# include <functional>
# include <optional>
# include <memory>
# include <vector>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

template<typename T>
struct object_cls {
	const std::type_info& type_info = typeid(T);
	std::function<std::string(const T&)> show = [](const T& x) {
		return std::to_string(x);
	};
	std::function<std::size_t(const T&)> hash = [](const T& x) {
		return std::hash<T>()(x);
	};
	std::function<bool(const T&, const T&)> equal = [](const T& x, const T& y) {
		return x == y;
	};
	std::function<void(std::ostream&, const T&)> encode =
		[](std::ostream& s, const T& x) { s << x; };
	std::function<T(std::istream&)> decode = [](std::istream& s) {
		T x;
		s >> x;
		return x;
	};
};

template<typename Key, typename Value>
struct rule_cls {
	const object_cls<Key>&   key_cls;
	const object_cls<Value>& value_cls;
	std::function<std::optional<Value>(const Key&)> stored = [](const Key&) {
		return std::nullopt;
	};
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

template<typename T>
struct proxy{};

template<typename Key, typename Value>
void
add_rule(
	rules& Rules,
	int aPrio,
	const std::function<bool(const Key&)>& aPred,
	const std::function<Value(acontext&, const Key&)>& anAction
	)
{
	const untyped_rule_cls& cls =
		untyped_rule_cls_instance(proxy<Key>{}, proxy<Value>{});

	unsafe_add_rule(Rules, cls, aPrio,
		[aPred](const untyped_key& aKey) {
			return aPred(*static_cast<const Key*>(aKey.get()));
		},
		[anAction](acontext& aCtx, const untyped_key& aKey) {
			return static_cast<untyped_value>(std::make_shared<Value>(
				anAction(aCtx, *static_cast<const Key*>(aKey.get()))));
		});
}

template<typename Key, typename Value>
std::vector<Value> apply_rules(acontext& aCtx, const std::vector<Key>& Keys)
{
	const untyped_rule_cls& cls =
		untyped_rule_cls_instance(proxy<Key>{}, proxy<Value>{});

	std::vector<untyped_key> untypedKeys;
	for (const Key& k : Keys) {
		untypedKeys.push_back(static_cast<untyped_key>(
			std::make_shared<Key>(k)));
	}

	std::vector<untyped_value> untypedValues =
		unsafe_apply_rules(aCtx, cls, untypedKeys);

	std::vector<Value> values;
	for (const untyped_value& untypedValue : untypedValues)
		values.push_back(*static_cast<Value*>(untypedValue.get()));
	return values;
}

template<typename Key, typename Value>
void apply_rules_(acontext& aCtx, const std::vector<Key>& Keys)
{
	const untyped_rule_cls& cls =
		untyped_rule_cls_instance(proxy<Key>{}, proxy<Value>{});

	std::vector<untyped_key> untypedKeys;
	for (const Key& k : Keys) {
		untypedKeys.push_back(static_cast<untyped_key>(
			std::make_shared<Key>(k)));
	}
	unsafe_apply_rules_(aCtx, cls, untypedKeys);
}

template<typename Key, typename Value>
Value apply_rule(acontext& aCtx, const Key& aKey)
{
	const untyped_rule_cls& cls =
		untyped_rule_cls_instance(proxy<Key>{}, proxy<Value>{});

	untyped_key untypedKey =
		static_cast<untyped_key>(std::make_shared<Key>(aKey));
	untyped_value untypedValue = unsafe_apply_rule(aCtx, cls, untypedKey);
	return *static_cast<Value*>(untypedValue.get());
}

template<typename Key, typename Value>
void apply_rule_(acontext& aCtx, const Key& aKey)
{
	const untyped_rule_cls& cls =
		untyped_rule_cls_instance(proxy<Key>{}, proxy<Value>{});

	untyped_key untypedKey =
		static_cast<untyped_key>(std::make_shared<Key>(aKey));
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
		.show = [&aCls](const untyped_object& anObj) {
			return aCls.show(*static_cast<const T*>(anObj.get()));
		},
		.hash = [&aCls](const untyped_object& anObj) {
			return aCls.hash(*static_cast<const T*>(anObj.get()));
		},
		.equal = [&aCls](const untyped_object& aLhs, const untyped_object& aRhs) {
			return aCls.equal(*static_cast<const T*>(aLhs.get()),
				*static_cast<const T*>(aRhs.get()));
		},
		.encode = [&aCls](std::ostream& aStrm, const untyped_object& anObj) {
			aCls.encode(aStrm, *static_cast<const T*>(anObj.get()));
		},
		.decode = [&aCls](std::istream& aStrm) {
			return static_cast<untyped_object>(std::make_shared<T>(
				aCls.decode(aStrm)));
		}
	};
}

template<typename Key, typename Value>
const untyped_rule_cls& untyped_rule_cls_instance(proxy<Key> k, proxy<Value> v)
{
	const rule_cls<Key, Value>& ruleCls = rule_cls_instance(k, v);
	static const untyped_object_cls untypedKeyCls =
		to_untyped_object_cls(ruleCls.key_cls);
	static const untyped_object_cls untypedValueCls =
		to_untyped_object_cls(ruleCls.value_cls);
	static const untyped_rule_cls untypedRuleCls = {
		.key_cls = untypedKeyCls,
		.value_cls = untypedValueCls,
		.stored = [&ruleCls](const untyped_key& aKey)
		{
			std::optional<Value> mbValue =
				ruleCls.stored(*static_cast<const Key*>(aKey.get()));
			return mbValue ? std::make_optional(static_cast<untyped_value>(std::make_shared<Value>(*mbValue)))
			               : std::nullopt;
		}
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

