
#include <bb.hpp>

struct Dummy {
	int x;
	inline bool operator== (const Dummy& o) const { return x == o.x; }
};

namespace std {

string to_string(const Dummy& d) { return to_string(d.x); }

template<>
struct std::hash<Dummy>
{
    inline std::size_t operator()(const Dummy& d) const noexcept { return d.x; }
};

}

#include <bb/rule.hpp>

static const bb::object_cls<Dummy>& object_cls_instance(bb::proxy<Dummy>)
{
	static const bb::object_cls<Dummy> cls;
	return cls;
}

static const bb::rule_cls<Dummy, Dummy>&
rule_cls_instance(bb::proxy<Dummy> k, bb::proxy<Dummy> v)
{
	static const bb::rule_cls<Dummy, Dummy> cls = {
		.key_cls = object_cls_instance(k),
		.value_cls = object_cls_instance(v)
	};
	return cls;
}

using bb::untyped_rule_cls_instance;

static void test_error_no_rule_to_build_type()
{
	int ecode = bb::run(bb::default_options, [](bb::rules& Rules) {
		bb::action(Rules, [](bb::acontext& aCtx) {
			bb::apply_rule_<Dummy, Dummy>(aCtx, Dummy{1});
		});
	});
	assert(0 < ecode);
}

static void test_error_no_rule()
{
	int ecode = bb::run(bb::default_options, [](bb::rules& Rules) {
		bb::want(Rules, { bb::filepath("build") / "foo" });
	});
	assert(0 < ecode);
}

static void test_error_recursion()
{
	int ecode = bb::run(bb::default_options, [](bb::rules& Rules) {
		bb::want(Rules, { bb::filepath("build") / "foo" });

		bb::file(Rules, "build/foo", [](bb::acontext& aCtx, const bb::filepath& aTarget) {
			(void) aTarget;
			bb::need(aCtx, { bb::filepath("build") / "bar" });
		});
		bb::file(Rules, "build/bar", [](bb::acontext& aCtx, const bb::filepath& aTarget) {
			(void) aTarget;
			bb::need(aCtx, { bb::filepath("build") / "foo" });
		});
	});
	assert(0 < ecode);
}

void test_errors()
{
	test_error_no_rule_to_build_type();
	test_error_no_rule();
	test_error_recursion();
}

