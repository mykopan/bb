
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
	size_t argszMax;

	argszMax = 0;
	for (const auto& arg : Args) {
		if (arg.second)
			argszMax = std::max(argszMax, arg.first.length());
	}

	errmsg << aMessage << ":" << std::endl;
	for (const auto& arg : Args) {
		if (arg.second) {
			errmsg << "  " << arg.first;
			if (!arg.first.empty())
				errmsg << ':';
			errmsg << std::string(2 + argszMax - arg.first.length(), ' ') <<
				*arg.second << std::endl;
		}
	}
	if (!aHint.empty())
		errmsg << aHint << std::endl;

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
error_symlink_not_file(const std::filesystem::path& aPath)
{
	return error_structured(
		"Build system error - expected a file, got a symlink",
		{ { "Symlink", aPath.string() } },
		"Probably due to calling bb::need on a symlink."
		" Currently, BB only permits bb::need on files.");
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
	const std::type_info& aTypeOfKey,
	const std::type_info& aTypeOfValue1,
	const std::type_info& aTypeOfValue2
	)
{
	return error_structured(
		"Build system error - detected rules with multiple result types",
		{ { "Key type", aTypeOfKey.name() }
		, { "First result type", aTypeOfValue1.name() }
		, { "Second result type", aTypeOfValue2.name() }
		},
		"A function passed to bb::add_rule has the wrong result type");
}

std::runtime_error
error_no_rule_to_build_type(
	const std::type_info& aTypeOfKey,
	const std::optional<std::string>& aKey,
	const std::type_info& aTypeOfValue
	)
{
	return error_structured(
		"Build system error - no rule matches the key type",
		{ { "Key type", aTypeOfKey.name() }
		, { "Key value", aKey }
		, { "Result type", aTypeOfValue.name() }
		},
		"Either you are missing a call to bb::add_rule, "
		"or your call to bb::apply_rule has the wrong key type");
}

std::runtime_error
error_rule_type_mismatch(
	const std::type_info& aTypeOfKey,
	const std::optional<std::string>& aKey,
	const std::type_info& aTypeOfValueReal,
	const std::type_info& aTypeOfValueWant
	)
{
	return error_structured(
		"Build system error - rule used at the wrong result type",
		{ { "Key type", aTypeOfKey.name() }
		, { "Key value", aKey }
		, { "Rule result type", aTypeOfValueReal.name() }
		, { "Requested result type", aTypeOfValueWant.name() }
		},
		"Either the function passed to bb::add_rule has the wrong result type, "
		"or the result of bb::apply_rule is used at the wrong type");
}

std::runtime_error
error_multiple_rules_match(
	const std::type_info& aTypeOfKey,
	const std::string& aKey,
	size_t aCount
	)
{
	return error_structured(
		std::string("Build system error - key matches ") +
			(0 == aCount ? "no" : "multiple") + " rules",
		{ { "Key type", aTypeOfKey.name() }
		, { "Key value", aKey }
		, { "Rules matched", std::to_string(aCount) }
		},
		0 == aCount
			? "Either add a rule that produces the above key, or stop requiring the above key"
			: "Modify your rules so only one can produce the above key");
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

