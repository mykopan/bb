
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULE_HPP__
# define __BB_RULE_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>
# include <functional>
# include <memory>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

template<typename T> using object = std::shared_ptr<T>;
template<typename T> using key    = object<T>;
template<typename T> using value  = object<T>;

template<typename T>
struct object_cls {
	const std::type_info&                                   type_info = typeid(T);
	std::function<std::string(const object<T>&)>            show;
	std::function<std::size_t(const object<T>&)>            hash;
	std::function<bool(const object<T>&, const object<T>&)> equal;
};

template<typename K, typename V>
struct rule_cls {
	const object_cls<K>& key_cls;
	const object_cls<V>& value_cls;
};


using untyped_object     = object<void>;
using untyped_key        = object<void>;
using untyped_value      = object<void>;
using untyped_object_cls = object_cls<void>;
using untyped_rule_cls   = rule_cls<void, void>;

template<typename T>
const untyped_object& to_untyped_object(const object<T>& aCls)
{
	return reinterpret_cast< const untyped_object& >(aCls);
}

template<typename T>
const untyped_object_cls& to_untyped_object_cls(const object_cls<T>& aCls)
{
	return reinterpret_cast< const untyped_object_cls& >(aCls);
}

template<typename K, typename V>
const untyped_rule_cls& to_untyped_rule_cls(const rule_cls<K, V>& aCls)
{
	return reinterpret_cast< const untyped_rule_cls& >(aCls);
}

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


template<typename K, typename V>
void
add_rule(
	rules& Rules,
	const rule_cls<K, V>& aCls,
	int aPrio,
	const std::function<bool(const key<K>&)>& aPred,
	const std::function<value<V>(acontext&, const key<K>&)>& anAction
	)
{
	unsafe_add_rule(Rules, to_untyped_rule_cls(aCls), aPrio,
		reinterpret_cast< const std::function<bool(const untyped_key&)>& >(aPred),
		reinterpret_cast< const std::function<untyped_value(acontext&, const untyped_key&)>& >(anAction));
}

template<typename K, typename V>
std::vector< value<V> >
apply_rules(
	acontext& aCtx,
	const rule_cls<K, V>& aCls,
	const std::vector< key<K> >& Keys
	)
{
	return unsafe_apply_rules(aCtx, to_untyped_rule_cls(aCls),
		reinterpret_cast< const std::vector<untyped_key>& >(Keys));
}

template<typename K, typename V>
void
apply_rules_(
	acontext& aCtx,
	const rule_cls<K, V>& aCls,
	const std::vector< key<K> >& Keys
	)
{
	unsafe_apply_rules_(aCtx, to_untyped_rule_cls(aCls),
		reinterpret_cast< const std::vector<untyped_key>& >(Keys));
}

template<typename K, typename V>
value<V>
apply_rule(
	acontext& aCtx,
	const rule_cls<K, V>& aCls,
	const key<K>& aKey
	)
{
	return unsafe_apply_rule(aCtx, to_untyped_rule_cls(aCls),
		reinterpret_cast< const untyped_key& >(aKey));
}

template<typename K, typename V>
void
apply_rule_(
	acontext& aCtx,
	const rule_cls<K, V>& aCls,
	const key<K>& aKey
	)
{
	unsafe_apply_rule_(aCtx, to_untyped_rule_cls(aCls),
		reinterpret_cast< const untyped_key& >(aKey));
}

}

#endif /* !defined(__BB_RULE_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

