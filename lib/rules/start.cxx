
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.h"
#include "bb/rules/start.h"

/*******************************************************************************
 * %% BeginSection: function definitions
 */
# pragma mark -
# pragma mark [ function definitions ]
# pragma mark -

namespace bb {

void action(rules& Rules, const std::function<void(acontext&)>& anAction)
{
	Rules.actions.push_back(anAction);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

