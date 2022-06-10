
/* -------------------------------------------------------------------------- */

#ifndef __BB_TYPES_I_HPP__
# define __BB_TYPES_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/types.hpp"
# include "bb/rule.hpp"
# include <vector>
# include <unordered_map>
# include <unordered_set>
# include <typeindex>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb {

struct dynamic_object {
	const untyped_object_cls& cls;
	untyped_object            obj;

	dynamic_object(const untyped_object_cls& aCls, const untyped_object& anObj)
		: cls(aCls), obj(anObj) {}

	inline const std::type_info& type_info() const
	{
		return (cls.type_info);
	}

	inline std::string show() const
	{
		return cls.show(obj);
	}

	inline std::size_t hash() const
	{
		return cls.hash(obj);
	}

	inline bool operator== (const dynamic_object& aRhs) const
	{
		return (type_info() == aRhs.type_info()
			&& cls.equal(obj, aRhs.obj));
	}
};

}

template<>
struct std::hash<bb::dynamic_object>
{
    inline std::size_t operator()(const bb::dynamic_object& x) const noexcept
    {
		return x.hash();
    }
};

namespace bb {

typedef dynamic_object dynamic_key;
typedef dynamic_object dynamic_value;

struct rule {
	rule(
		const std::function<bool(const untyped_key&)>& aPred,
		const std::function<untyped_value(acontext&, const untyped_key&)>& anAction
		)
		: predicate(aPred), action(anAction)
	{}

	std::function<bool(const untyped_key&)>                     predicate;
	std::function<untyped_value(acontext&, const untyped_key&)> action;
};

struct prule {
	prule(int aPrio, const rule& aRule)
		: priority(aPrio), rule(aRule)
	{}

	int   priority;
	rule  rule;
};

struct crule {
	crule(const untyped_rule_cls& aCls, const std::vector<prule>& PRules)
		: cls(aCls), prules(PRules)
	{}

	const untyped_rule_cls& cls;
	std::vector<prule>      prules;
};

struct rules {
	std::vector< std::function<void(acontext&)> > actions;
	std::unordered_map<std::type_index, crule>    crules;
};

struct acontext {
	acontext(
		const options& Options,
		const rules& Rules,
		std::unordered_set<dynamic_key>& InprogressKeys
		)
		: options(Options)
		, rules(Rules)
		, inprogress_keys(InprogressKeys)
		, stack()
	{}

	const options&                    options;
	const rules&                      rules;
	std::unordered_set<dynamic_key>&  inprogress_keys;
	std::vector<dynamic_key>          stack;
};

}

#endif /* !defined(__BB_TYPES_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

