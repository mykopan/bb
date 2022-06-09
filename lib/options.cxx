
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/run.hpp"
#include <iostream>

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

static options _s_options = {
	.files = "build",
	.nthreads = 1,
	.version = "1",
	.verbosity = INFO,
	.staunch = false,
	.change = MODTIME,
	.output = [](verbosity, const std::string& aMsg) {
		std::cout << aMsg << std::endl;
	}
};
const options& default_options = _s_options;

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

