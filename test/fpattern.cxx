
#include <bb/rules/file.hpp>

void test_file_pattern()
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

