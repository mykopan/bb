
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.h"
#include "bb/log.h"

/*******************************************************************************
 * %% BeginSection: function definitions
 */
# pragma mark -
# pragma mark [ function definitions ]
# pragma mark -

namespace bb {

void verbose(acontext& aCtx, const std::string& aMsg)
{
	aCtx.options.output(VERBOSE, aMsg);
}

void info(acontext& aCtx, const std::string& aMsg)
{
	aCtx.options.output(INFO, aMsg);
}

void warn(acontext& aCtx, const std::string& aMsg)
{
	aCtx.options.output(WARN, aMsg);
}

void error(acontext& aCtx, const std::string& aMsg)
{
	aCtx.options.output(ERROR, aMsg);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

