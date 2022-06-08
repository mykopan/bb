
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULES_START_H__
# define __BB_RULES_START_H__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.h>
# include <functional>

/*******************************************************************************
 * %% BeginSection: function declarations
 */
# pragma mark -
# pragma mark [ function declarations ]
# pragma mark -

namespace bb {

/* Run an action, usually used for specifying top-level requirements. */
extern void action(rules&, const std::function<void(acontext&)>&);

}

#endif /* !defined(__BB_RULES_START_H__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

