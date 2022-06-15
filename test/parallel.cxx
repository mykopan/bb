
#include <bb.hpp>
#include <bb/rule.hpp>

struct wint { int x; };

static const bb::object_cls<wint>& object_cls_instance(bb::proxy<wint>)
{
	static const bb::object_cls<wint> cls = {
		.show = [](const wint& w) { return std::to_string(w.x); },
		.hash = [](const wint& w) { return w.x; },
		.equal = [](const wint& x, const wint& y) { return x.x == y.x; }
	};
	return cls;
}

static const bb::rule_cls<wint, wint>&
rule_cls_instance(bb::proxy<wint> k, bb::proxy<wint> v)
{
	static const bb::rule_cls<wint, wint> cls = {
		.key_cls = object_cls_instance(k),
		.value_cls = object_cls_instance(v)
	};
	return cls;
}

using bb::untyped_rule_cls_instance;

static void test_parallel_simple()
{
	bb::options opts = bb::default_options;
	opts.nthreads = 0;
	const int n = 10000;

	int ecode = bb::run(opts, [](bb::rules& Rules) {
		bb::action(Rules, [](bb::acontext& aCtx) {
			std::vector<wint> ks;
			ks.reserve(n);
			for (int k = 0; k < n; ++k)
				ks.push_back(wint{k});
			auto vs = bb::apply_rules<wint, wint>(aCtx, ks);
			for (int k = 0; k < n; ++k)
				assert(2 * k == vs[k].x);
		});

		bb::add_rule<wint, wint>(Rules, 1,
			[](const wint&) { return true; },
			[](bb::acontext&, const wint& k) { return wint{ 2 * k.x }; }
		);
	});
	assert(0 == ecode);
}

static void test_parallel_fibonacci()
{
	bb::options opts = bb::default_options;
	opts.nthreads = 0;
	const int n = 16;

	int ecode = bb::run(opts, [](bb::rules& Rules) {
		bb::action(Rules, [](bb::acontext& aCtx) {
			std::vector<wint> ks;
			ks.reserve(n);
			for (int k = 0; k < n; ++k)
				ks.push_back(wint{k});
			auto vs = bb::apply_rules<wint, wint>(aCtx, ks);
			for (int k = 0; k < n; ++k) {
				bb::info(aCtx, std::string("fibonacci ") + std::to_string(k) +
					" = " + std::to_string(vs[k].x));
			}
		});

		bb::add_rule<wint, wint>(Rules, 1,
			[](const wint& k) { return 0 == k.x || 1 == k.x; },
			[](bb::acontext&, const wint& k) { return k; }
		);
		bb::add_rule<wint, wint>(Rules, 1,
			[](const wint& k) { return 2 <= k.x; },
			[](bb::acontext& aCtx, const wint& k) {
				auto vs = bb::apply_rules<wint, wint>(aCtx,
					{ wint{k.x - 2}, wint{k.x - 1} });
				return wint{vs[0].x + vs[1].x};
			}
		);
	});
	assert(0 == ecode);
}

void test_parallel()
{
	test_parallel_simple();
	test_parallel_fibonacci();
}

