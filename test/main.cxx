
#include <bb.hpp>

static void test_recursion()
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

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;
	test_recursion();
}

