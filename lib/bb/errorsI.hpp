
/* -------------------------------------------------------------------------- */

#ifndef __BB_ERRORS_I_HPP__
# define __BB_ERRORS_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <filesystem>
# include <optional>
# include <stdexcept>
# include <string>
# include <typeinfo>
# include <utility>
# include <vector>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */
# pragma mark -
# pragma mark [ type/constant declarations ]
# pragma mark -

namespace bb {

extern std::runtime_error
error_structured(
	const std::string& aMessage,
	const std::vector< std::pair< std::string, std::optional<std::string> > >& Args,
	const std::string& aHint
	);

extern std::runtime_error
error_directory_not_file(const std::filesystem::path&);

extern std::runtime_error
error_symlink_not_file(const std::filesystem::path&);

extern std::runtime_error
error_not_file(const std::filesystem::path&);

extern std::runtime_error
error_incompatible_rules(
	const std::type_info& aTypeOfKey,
	const std::type_info& aTypeOfValue1,
	const std::type_info& aTypeOfValue2
	);

extern std::runtime_error
error_no_rule_to_build_type(
	const std::type_info& aTypeOfKey,
	const std::optional<std::string>& aKey,
	const std::type_info& aTypeOfValue
	);

extern std::runtime_error
error_rule_type_mismatch(
	const std::type_info& aTypeOfKey,
	const std::optional<std::string>& aKey,
	const std::type_info& aTypeOfValueReal,
	const std::type_info& aTypeOfValueWant
	);

extern std::runtime_error
error_multiple_rules_match(
	const std::type_info& aTypeOfKey,
	const std::string& aKey,
	size_t aCount
	);

}

#endif /* !defined(__BB_ERRORS_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
