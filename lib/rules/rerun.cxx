
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/rule.hpp"
#include "bb/rules/rerun.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

struct rerunQ {};

static const object_cls<rerunQ>& object_cls_instance(proxy<rerunQ>)
{
	static const object_cls<rerunQ> cls = {
		.show = [](const rerunQ&) { return "rerunQ{}"; },
		.hash = [](const rerunQ&) { return 0; },
		.equal = [](const rerunQ&, const rerunQ&) { return true; },
		.encode = [](std::ostream&, const rerunQ&) {},
		.decode = [](std::istream&) { return rerunQ{}; }
	};
	return cls;
}

struct rerunA {};

static const object_cls<rerunA>& object_cls_instance(proxy<rerunA>)
{
	static const object_cls<rerunA> cls = {
		.show = [](const rerunA&) { return "rerunA{}"; },
		.hash = [](const rerunA&) { return 0; },
		.equal = [](const rerunA&, const rerunA&) { return false; },
		.encode = [](std::ostream&, const rerunA&) {},
		.decode = [](std::istream&) { return rerunA{}; }
	};
	return cls;
}

static const rule_cls<rerunQ, rerunA>&
rule_cls_instance(proxy<rerunQ> k, proxy<rerunA> v)
{
	static const rule_cls<rerunQ, rerunA> cls = {
		.key_cls = object_cls_instance(k),
		.value_cls = object_cls_instance(v)
	};
	return cls;
}

void always_rerun(acontext& aCtx)
{
	apply_rule_<rerunQ, rerunA>(aCtx, rerunQ{});
}

void default_rerun_rule(rules& Rules)
{
	add_rule<rerunQ, rerunA>(Rules, 0,
		[](const rerunQ&) { return true; },
		[](acontext&, const rerunQ&) { return rerunA{}; });
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

