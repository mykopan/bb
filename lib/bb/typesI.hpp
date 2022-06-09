
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

struct Object {
	const object_cls& cls;
	object            obj;

	Object(const object_cls& aCls, const object& anObj)
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

	inline bool operator== (const Object& aRhs) const
	{
		return (type_info() == aRhs.type_info()
			&& cls.equal(obj, aRhs.obj));
	}
};

}

template<>
struct std::hash<bb::Object>
{
    inline std::size_t operator()(const bb::Object& x) const noexcept
    {
		return x.hash();
    }
};

namespace bb {

typedef Object Key;
typedef Object Value;

struct rule {
	rule(
		const std::function<bool(const key&)>& aPred,
		const std::function<value(acontext&, const key&)>& anAction
		)
		: predicate(aPred), action(anAction)
	{}

	std::function<bool(const key&)>             predicate;
	std::function<value(acontext&, const key&)> action;
};

struct prule {
	prule(int aPrio, const rule& aRule)
		: priority(aPrio), rule(aRule)
	{}

	int   priority;
	rule  rule;
};

struct crule {
	crule(const rule_cls& aCls, const std::vector<prule>& PRules)
		: cls(aCls), prules(PRules)
	{}

	const rule_cls&     cls;
	std::vector<prule>  prules;
};

struct rules {
	std::vector< std::function<void(acontext&)> > actions;
	std::unordered_map<std::type_index, crule>    crules;
};

struct acontext {
	acontext(
		const options& Options,
		const rules& Rules,
		std::unordered_set<Key>& InprogressKeys
		)
		: options(Options)
		, rules(Rules)
		, inprogress_keys(InprogressKeys)
	{}

	const options&            options;
	const rules&              rules;
	std::unordered_set<Key>&  inprogress_keys;
};

}

#endif /* !defined(__BB_TYPES_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

