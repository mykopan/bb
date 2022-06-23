
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/rule.hpp"
#include "bb/rules/orderonly.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

struct order_onlyQ {
	order_onlyQ(const filepath& aPath)
		: file_path(aPath)
	{}

	filepath file_path;
};

static const object_cls<order_onlyQ>& object_cls_instance(proxy<order_onlyQ>)
{
	static const object_cls<order_onlyQ> cls = {
		.show = [](const order_onlyQ& aFileQ) {
			return aFileQ.file_path.string();
		},
		.hash = [](const order_onlyQ& aFileQ) {
			return std::hash<std::string>{}(aFileQ.file_path);
		},
		.equal = [](const order_onlyQ& aLhs, const order_onlyQ& aRhs) {
			return aLhs.file_path == aRhs.file_path;
		},
		.encode = [](std::ostream& aStrm, const order_onlyQ& aFileQ) {
			encode(aStrm, aFileQ.file_path.string());
		},
		.decode = [](std::istream& aStrm) {
			std::string pth;
			decode(aStrm, pth);
			return order_onlyQ(pth);
		}
	};
	return cls;
}

struct order_onlyA {};

static const object_cls<order_onlyA>& object_cls_instance(proxy<order_onlyA>)
{
	static const object_cls<order_onlyA> cls = {
		.show = [](const order_onlyA&) { return "order_onlyA{}"; },
		.hash = [](const order_onlyA&) { return 0; },
		.equal = [](const order_onlyA&, const order_onlyA&) { return true; },
		.encode = [](std::ostream&, const order_onlyA&) {},
		.decode = [](std::istream&) { return order_onlyA{}; }
	};
	return cls;
}

static const rule_cls<order_onlyQ, order_onlyA>&
rule_cls_instance(proxy<order_onlyQ> k, proxy<order_onlyA> v)
{
	static const rule_cls<order_onlyQ, order_onlyA> cls = {
		.key_cls = object_cls_instance(k),
		.value_cls = object_cls_instance(v),
		.stored = [](const order_onlyQ&) { return order_onlyA{}; }
	};
	return cls;
}

void order_only(acontext& aCtx, const std::vector<filepath>& Files)
{
	std::vector<order_onlyQ> qFiles;
	for (const auto& file : Files)
		qFiles.push_back(order_onlyQ(file));
	apply_rules_<order_onlyQ, order_onlyA>(aCtx, qFiles);
}

void default_order_only_rule(rules& Rules)
{
	add_rule<order_onlyQ, order_onlyA>(Rules, 0,
		[](const order_onlyQ&) { return true; },
		[](acontext& aCtx, const order_onlyQ& aFileQ) {
			need(aCtx, { aFileQ.file_path });
			return order_onlyA{};
		});
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

