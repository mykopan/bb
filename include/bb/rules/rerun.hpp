
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULES_RERUN_HPP__
# define __BB_RULES_RERUN_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

/* Always rerun the associated action. Useful for defining rules that query the
 * environment. For example:
 *
 * bb::file(Rules, "gcc-version.txt", [](acontext& aCtx, const bb::filepath& aDst) {
 *     bb::always_rerun(aCtx);
 *     std::string stdout;
 *     bb::cmd(bb::stdout{stdout}, "gcc", "--version");
 *     bb::write_file_changed(aDst, stdout);
 * });
 *
 * In make, the .PHONY attribute on file-producing rules has a similar effect.
 *
 * Note that bb::always_rerun is applied when a rule is executed. Modifying an
 * existing rule to insert bb::always_rerun will not cause that rule to rerun
 * next time. */
extern void always_rerun(acontext&);

extern void default_rerun_rule(rules&);

}

#endif /* !defined(__BB_RULES_RERUN_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

