
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/errorsI.hpp"
#include <sstream>

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

std::runtime_error
error_structured(
	const std::string& aMessage,
	const std::vector< std::pair< std::string, std::optional<std::string> > >& Args,
	const std::string& aHint
	)
{
	std::stringstream errmsg;
	size_t argszMax, nargs;

	argszMax = nargs = 0;
	for (const auto& arg : Args) {
		if (arg.second) {
			argszMax = std::max(argszMax, arg.first.length());
			++nargs;
		}
	}

	errmsg << aMessage;
	if (0 < nargs || !aHint.empty())
		errmsg << ':';

	for (const auto& arg : Args) {
		if (arg.second) {
			errmsg << std::endl << "  " << arg.first;
			if (!arg.first.empty())
				errmsg << ':';
			errmsg << std::string(2 + argszMax - arg.first.length(), ' ') <<
				*arg.second;
		}
	}
	if (!aHint.empty())
		errmsg << std::endl << aHint;

	return std::runtime_error(errmsg.str());
}

std::runtime_error
error_directory_not_file(const std::filesystem::path& aDir)
{
	return error_structured(
		"Build system error - expected a file, got a directory",
		{ { "Directory", aDir.string() } },
		"Probably due to calling bb::need on a directory."
		" BB only permits bb::need on files.");
}

std::runtime_error
error_not_file(const std::filesystem::path& aPath)
{
	return error_structured(
		"Build system error - expected a file, got a something else",
		{ { "Path to thing", aPath.string() } },
		"Probably due to calling bb::need on a non file thing."
		" BB only permits bb::need on files.");
}

std::runtime_error
error_incompatible_rules(
	const std::string& aTypeOfKey,
	const std::string& aTypeOfValue1,
	const std::string& aTypeOfValue2
	)
{
	return error_structured(
		"Build system error - detected rules with multiple result types",
		{ { "Key type", aTypeOfKey }
		, { "First result type", aTypeOfValue1 }
		, { "Second result type", aTypeOfValue2 }
		},
		"A function passed to bb::add_rule has the wrong result type");
}

std::runtime_error
error_no_rule_to_build_type(
	const std::string& aTypeOfKey,
	const std::optional<std::string>& aKey,
	const std::optional<std::string>& aTypeOfValue
	)
{
	return error_structured(
		"Build system error - no rule matches the key type",
		{ { "Key type", aTypeOfKey }
		, { "Key value", aKey }
		, { "Result type", aTypeOfValue }
		},
		"Either you are missing a call to bb::add_rule, "
		"or your call to bb::apply_rule has the wrong key type");
}

std::runtime_error
error_rule_type_mismatch(
	const std::string& aTypeOfKey,
	const std::optional<std::string>& aKey,
	const std::string& aTypeOfValueReal,
	const std::string& aTypeOfValueWant
	)
{
	return error_structured(
		"Build system error - rule used at the wrong result type",
		{ { "Key type", aTypeOfKey }
		, { "Key value", aKey }
		, { "Rule result type", aTypeOfValueReal }
		, { "Requested result type", aTypeOfValueWant }
		},
		"Either the function passed to bb::add_rule has the wrong result type, "
		"or the result of bb::apply_rule is used at the wrong type");
}

std::runtime_error
error_multiple_rules_match(
	const std::string& aTypeOfKey,
	const std::string& aKey,
	size_t aCount
	)
{
	return error_structured(
		std::string("Build system error - key matches ") +
			(0 == aCount ? "no" : "multiple") + " rules",
		{ { "Key type", aTypeOfKey }
		, { "Key value", aKey }
		, { "Rules matched", std::to_string(aCount) }
		},
		0 == aCount
			? "Either add a rule that produces the above key, or stop requiring the above key"
			: "Modify your rules so only one can produce the above key");
}

std::runtime_error
error_rule_recursion(
	const std::string& aTypeOfKey,
	const std::string& aKey
	)
{
	return error_structured(
		"Build system error - recursion detected",
		{ { "Key type", aTypeOfKey }
		, { "Key value", aKey }
		},
		"Rules may not be recursive");
}

bb_exception::bb_exception(
	const std::vector<std::string>& aStack,
	const std::exception& anInnerException
	)
{
	std::stringstream errmsg;

	errmsg << "Error when running build system:" << std::endl;
	for (const auto& key : aStack)
		errmsg << "* " << key << std::endl;
	errmsg << anInnerException.what();

	this->errmsg = errmsg.str();
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

