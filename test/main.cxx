
int main(int argc, char *argv[])
{
	extern void test_file_pattern(),
		test_async(),
		test_errors(),
		test_parallel();

	(void) argc;
	(void) argv;

	test_file_pattern();
	test_async();
	test_errors();
	// test_parallel();
}

