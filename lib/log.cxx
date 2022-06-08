
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/log.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

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

