
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULES_FILE_HPP__
# define __BB_RULES_FILE_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>
# include <functional>
# include <vector>
# include <string>
# include <filesystem>

/*******************************************************************************
 * %% BeginSection: function declarations
 */
# pragma mark -
# pragma mark [ function declarations ]
# pragma mark -

namespace bb {

typedef std::filesystem::path filepath;
typedef std::string           fpattern;

/* Add a dependency on the file arguments, ensuring they are built before
 * continuing. The file arguments may be built in parallel, in any order.
 * This function is particularly necessary when calling bb::cmd. */
extern void need(acontext&, const std::vector<filepath>&);

/* Require that the argument files are built by the rules, used to specify
 * the target.
 *
 * This function is defined in terms of bb::action and bb::need, use bb::action
 * if you need more complex targets than want allows. */
extern void want(rules&, const std::vector<filepath>&);

/* Define a rule that matches a bb::fpattern, see bb::is_subset_fpattern for
 * the pattern rules. Patterns with no wildcards have higher priority than
 * those with wildcards, and no file required by the system may be matched by
 * more than one pattern at the same priority (see bb::priority and
 * bb::alternatives to modify this behaviour). This function will create the
 * directory for the result file, if necessary. */
extern void file(rules&, const fpattern&, const std::function<void(acontext&, const filepath&)>&);

/* Match a bb:fpattern against a bb::filepath, There are three special forms:
 * * matches an entire path component, excluding any separators.
 * // matches an arbitrary number of path components, including absolute path
 * prefixes.
 */
extern bool is_subset_fpattern(const filepath&, const fpattern&);

/* Default file rule which check a required file on existing. */
extern void default_file_rule(rules&);

}

#endif /* !defined(__BB_RULES_FILE_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

