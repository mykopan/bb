
#include <bb.hpp>
#include <bb/rule.hpp>
#include <iostream>

static void test_file_pattern()
{
	assert(bb::is_subset_fpattern("foo/bar/baz.c", "//*.c"));
	assert(bb::is_subset_fpattern("foo/bar/baz.c", "**/*.c"));
	assert(bb::is_subset_fpattern("baz.c", "*.c"));
	assert(bb::is_subset_fpattern("baz.c", "//*.c"));
	assert(bb::is_subset_fpattern("baz.c", "**/*.c"));
	assert(bb::is_subset_fpattern("baz.c", "baz.c"));

	assert(!bb::is_subset_fpattern("foo/bar.c", "*.c"));
	assert(!bb::is_subset_fpattern("foo/bar/baz.c", "*/*.c"));
	assert(!bb::is_subset_fpattern("foobar", "foo//bar"));
	assert(!bb::is_subset_fpattern("foobar", "foo/**/bar"));
	assert(!bb::is_subset_fpattern("foobar/bar", "foo//bar"));
	assert(!bb::is_subset_fpattern("foobar/bar", "foo/**/bar"));
	assert(!bb::is_subset_fpattern("foo/foobar", "foo/**/bar"));

	assert(bb::is_subset_fpattern("foo/bar", "foo//bar"));
	assert(bb::is_subset_fpattern("foo/bar", "foo/**/bar"));
	assert(bb::is_subset_fpattern("/foo", "//*"));
	assert(!bb::is_subset_fpattern("/foo", "**/*"));
	assert(bb::is_subset_fpattern("/bob/this/test/foo", "/bob//foo"));
	assert(bb::is_subset_fpattern("/bob/this/test/foo", "/bob/**/foo"));
	assert(!bb::is_subset_fpattern("bob/this/test/foo", "/bob//foo"));
	assert(!bb::is_subset_fpattern("bob/this/test/foo", "/bob/**/foo"));
	assert(bb::is_subset_fpattern("bob/this/test/foo/", "bob//foo/"));
	assert(bb::is_subset_fpattern("bob/this/test/foo/", "bob/**/foo/"));
	assert(!bb::is_subset_fpattern("bob/this/test/foo", "bob//foo/"));
	assert(!bb::is_subset_fpattern("bob/this/test/foo", "bob/**/foo/"));

	assert(bb::is_subset_fpattern("a", "a//"));
	assert(bb::is_subset_fpattern("a", "a/**"));
	assert(bb::is_subset_fpattern("/a", "/a//"));
	assert(bb::is_subset_fpattern("/a", "/a/**"));
	assert(bb::is_subset_fpattern("/a", "///a//"));
	assert(!bb::is_subset_fpattern("/a", "**/a/**"));
	assert(!bb::is_subset_fpattern("", "///"));
	assert(bb::is_subset_fpattern("/", "///"));
	assert(bb::is_subset_fpattern("a/", "///"));
	assert(bb::is_subset_fpattern("a/", "**/"));
	assert(bb::is_subset_fpattern("", "////"));
	assert(bb::is_subset_fpattern("", "**/**"));
	assert(bb::is_subset_fpattern("x/y", "x///y"));
	assert(bb::is_subset_fpattern("x/y", "x/**/y"));
	assert(bb::is_subset_fpattern("x/", "x///"));
	assert(bb::is_subset_fpattern("x/", "x/**/"));
	assert(bb::is_subset_fpattern("x/foo/", "x///"));
	assert(bb::is_subset_fpattern("x/foo/", "x/**/"));
	assert(!bb::is_subset_fpattern("x", "x///"));
	assert(!bb::is_subset_fpattern("x", "x/**/"));
	assert(bb::is_subset_fpattern("x/foo/bar/", "x///"));
	assert(bb::is_subset_fpattern("x/foo/bar/", "x/**/"));
	assert(!bb::is_subset_fpattern("x/foo/bar", "x///"));
	assert(!bb::is_subset_fpattern("x/foo/bar", "x/**/"));
	assert(bb::is_subset_fpattern("x/z/y", "x///y"));
	assert(bb::is_subset_fpattern("x/z/y", "x/**/*/y"));
	assert(bb::is_subset_fpattern("", ""));
	assert(!bb::is_subset_fpattern("y", ""));
	assert(!bb::is_subset_fpattern("/", ""));

	assert(bb::is_subset_fpattern("x/y", "*/*"));
	assert(!bb::is_subset_fpattern("x", "*/*"));
	assert(bb::is_subset_fpattern("x", "//*"));
	assert(bb::is_subset_fpattern("x", "**/*"));
	assert(bb::is_subset_fpattern("", "//*"));
	assert(bb::is_subset_fpattern("", "**/*"));
	assert(bb::is_subset_fpattern("x", "*//"));
	assert(bb::is_subset_fpattern("x", "*/**"));
	assert(bb::is_subset_fpattern("", "*//"));
	assert(bb::is_subset_fpattern("", "*/**"));
	assert(bb::is_subset_fpattern("x/y", "*//*"));
	assert(bb::is_subset_fpattern("x/y", "*/**/*"));
	assert(!bb::is_subset_fpattern("", "*//*"));
	assert(!bb::is_subset_fpattern("", "*/**/*"));
	assert(!bb::is_subset_fpattern("x", "*//*"));
	assert(!bb::is_subset_fpattern("x", "*/**/*"));
	assert(!bb::is_subset_fpattern("x/y", "*//*//*"));
	assert(!bb::is_subset_fpattern("x/y", "*/**/*/**/*"));
	assert(bb::is_subset_fpattern("/", "//*/"));
	assert(!bb::is_subset_fpattern("/", "**/*/"));
	assert(bb::is_subset_fpattern("/", "*/////"));
	assert(bb::is_subset_fpattern("/", "*/**/**/"));
	assert(!bb::is_subset_fpattern("bb", "b*b*b*//"));
	assert(!bb::is_subset_fpattern("bb", "b*b*b*/**"));

	assert(!bb::is_subset_fpattern("/", "**"));
	assert(!bb::is_subset_fpattern("/x", "**/x"));
	assert(bb::is_subset_fpattern("x/", "**"));

	assert(bb::is_subset_fpattern("file", "./file"));
	assert(bb::is_subset_fpattern("foo/bar", "foo/./bar"));
	assert(bb::is_subset_fpattern("foo/./bar", "foo/./bar"));
	assert(!bb::is_subset_fpattern("foo/bob", "foo/./bar"));
}

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

static void test_error_no_rule_to_build_type()
{
	int ecode = bb::run(bb::default_options, [](bb::rules& Rules) {
		bb::action(Rules, [](bb::acontext& aCtx) {
			bb::apply_rule_<Dummy, Dummy>(aCtx, dummy_rcls,
				std::make_shared<Dummy>(Dummy{1}));
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

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;
	test_file_pattern();
	test_error_no_rule_to_build_type();
	test_error_no_rule();
	test_error_recursion();
	test_file_pattern();
}

