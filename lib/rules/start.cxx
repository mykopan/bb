
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/rules/start.hpp"

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

