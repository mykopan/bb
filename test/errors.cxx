
#include <bb.hpp>
#include <bb/rule.hpp>

struct Dummy {
	int x;
	bool operator== (const Dummy& o) { return (x == o.x); }
};

namespace std {

string to_string(const Dummy& d) { return to_string(d.x); }

template<>
struct std::hash<Dummy>
{
    inline std::size_t operator()(const Dummy& d) const noexcept { return d.x; }
};

}

template<typename T>
bb::object_cls<T> make_default_cls()
{
	return bb::object_cls<T>(
		[](const bb::object<T>& x) { return std::to_string(*x); },
		[](const bb::object<T>& x) { return std::hash<T>()(*x); },
		[](const bb::object<T>& x, const bb::object<T>& y) { return (*x == *y); }
	);
}

static bb::object_cls<Dummy> dummy_cls = make_default_cls<Dummy>();
static bb::rule_cls<Dummy, Dummy> dummy_rcls = { dummy_cls, dummy_cls };

void test_error_no_rule_to_build_type()
{
	int ecode = bb::run(bb::default_options, [](bb::rules& Rules) {
		bb::action(Rules, [](bb::acontext& aCtx) {
			bb::apply_rule_<Dummy, Dummy>(aCtx, dummy_rcls,
				std::make_shared<Dummy>(Dummy{1}));
		});
	});
	assert(0 < ecode);
}

void test_error_no_rule()
{
	int ecode = bb::run(bb::default_options, [](bb::rules& Rules) {
		bb::want(Rules, { bb::filepath("build") / "foo" });
	});
	assert(0 < ecode);
}

void test_error_recursion()
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

