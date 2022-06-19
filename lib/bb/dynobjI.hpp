
/* -------------------------------------------------------------------------- */

#ifndef __BB_DYNOBJ_I_HPP__
# define __BB_DYNOBJ_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/rule.hpp"

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb {

struct dynamic_object {
	const untyped_object_cls* cls;
	untyped_object            obj;

	dynamic_object(const untyped_object_cls& aCls, const untyped_object& anObj)
		: cls(&aCls), obj(anObj) {}
	dynamic_object(const dynamic_object& aSrc)
		: cls(aSrc.cls), obj(aSrc.obj) {}

	dynamic_object& operator= (const dynamic_object& aSrc)
	{
		cls = aSrc.cls;
		obj = aSrc.obj;
		return *this;
	}

	inline const std::type_info& type_info() const
	{
		return (cls->type_info);
	}

	inline std::string show() const
	{
		return cls->show(obj);
	}

	inline std::size_t hash() const
	{
		return cls->hash(obj);
	}

	inline bool operator== (const dynamic_object& aRhs) const
	{
		return (type_info() == aRhs.type_info()
			&& cls->equal(obj, aRhs.obj));
	}
};

typedef dynamic_object dynamic_key;
typedef dynamic_object dynamic_value;

}

template<>
struct std::hash<bb::dynamic_object>
{
    inline std::size_t operator()(const bb::dynamic_object& x) const noexcept
    {
		return x.hash();
    }
};

#endif /* !defined(__BB_DYNOBJ_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

