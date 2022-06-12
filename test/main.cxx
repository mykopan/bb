
int main(int argc, char *argv[])
{
	extern void test_file_pattern(),
		test_error_no_rule_to_build_type(),
		test_error_no_rule(),
		test_error_recursion();

	(void) argc;
	(void) argv;

	test_file_pattern();
	test_error_no_rule_to_build_type();
	test_error_no_rule();
	test_error_recursion();
}

