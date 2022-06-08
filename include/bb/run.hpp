
/* -------------------------------------------------------------------------- */

#ifndef __BB_RUN_HPP__
# define __BB_RUN_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>
# include <functional>

/*******************************************************************************
 * %% BeginSection: function declarations
 */
# pragma mark -
# pragma mark [ function declarations ]
# pragma mark -

namespace bb {

extern const options& default_options;

/* Main entry point for running build systems. Use bb::options to specify
 * how the system runs, and callback to specify what to build. The function
 * will return >0 if the build fails.
 *
 * To use command line flags to modify bb::options see bb::args. */
extern int run(const options&, const std::function<void(rules&)>&);

extern int args(int, char*[], const options&, const std::function<void(rules&)>&);

}

#endif /* !defined(__BB_RUN_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

