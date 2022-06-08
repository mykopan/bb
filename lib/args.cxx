
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/run.h"

/*******************************************************************************
 * %% BeginSection: function definitions
 */
# pragma mark -
# pragma mark [ function definitions ]
# pragma mark -

namespace bb {

int
args(
	int anArgc,
	char* anArgv[],
	const options& Options,
	const std::function<void(rules&)>& aRulesGen
	)
{
	(void) anArgc;
	(void) anArgv;

	return run(Options, aRulesGen);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

