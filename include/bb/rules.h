
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULES_H__
# define __BB_RULES_H__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.h>
# include <bb/rules/start.h>
# include <bb/rules/file.h>
# include <functional>

/*******************************************************************************
 * %% BeginSection: function declarations
 */
# pragma mark -
# pragma mark [ function declarations ]
# pragma mark -

namespace bb {

/* Change the priority of a given set of rules, where higher priorities take
 * precedence. All matching rules at a given priority must be disjoint,
 * or an error is raised. All builtin bb rules have priority between 0 and 1.
 * Excessive use of priority is discouraged. As an example:
 *
 * priority(Rules, 4, [](rules& Rules) {
 *     file(Rules, "hello.*", ...);
 * });
 * priority(Rules, 8, [](rules& Rules) {
 *     file(Rules, "*.txt", ...);
 * });
 *
 * In this example hello.txt will match the second rule, instead of raising
 * an error about ambiguity.
 *
 * The priority function obeys the invariants:
 *
 * priority(Rules, p1, [](rules& Rules){ priority(Rules, p2, r1); });
 *     === priority(Rules, p1, r1);
 * priority(Rules, p1, [](rules& Rules){ r1(Rules); r2(Rules); });
 *     === priority(Rules, p1, r1); priority(Rules, p1, r2);
 */
extern void priority(rules&, int, const std::function<void(rules&)>&);

}

#endif /* !defined(__BB_RULE_H__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

