
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULES_ORDERONLY_HPP__
# define __BB_RULES_ORDERONLY_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/rules/file.hpp>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

/* Define order-only dependencies, these are dependencies that will always be
 * built before continuing, but which aren't dependencies of this action.
 * Mostly useful for defining generated dependencies you think might be real
 * dependencies. If they turn out to be real dependencies, you should add
 * an explicit dependency afterwards.
 *
 * bb::file(Rules, "source.o", [](acontext& aCtx, const bb::filepath& anOut) {
 *     bb::order_only(aCtx, { "header.h" });
 *     bb::cmd("gcc -c source.c -o source.o -MMD -MF source.m");
 *     bb::needed_makefile_dependencies(aCtx, "source.m");
 * });
 *
 * If header.h is included by source.c then the call to
 * bb::needed_makefile_dependencies will cause it to be added as a real
 * dependency. If it isn't, then the rule won't rebuild if it changes. */
extern void order_only(acontext&, const std::vector<filepath>&);

extern void default_order_only_rule(rules&);

}

#endif /* !defined(__BB_RULES_ORDERONLY_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

