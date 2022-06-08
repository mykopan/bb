
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/command.hpp"
#include "bb/log.hpp"

#include <sstream>

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

void run_cmd(acontext& aCtx, const std::vector<std::string>& Args)
{
	if (Args.empty())
		return;

	(void) aCtx;
	// info(aCtx, "# " + Args[0]); // @todo: add: (for aCtx.stack[TOP].show())

	pid_t child = fork();
	if (child < 0) {
		throw std::runtime_error(std::string(strerror(errno)));
		/* NOTREACHED */
	}

	if (0 == child) {
		const char **argv;
		size_t i;

		argv = (const char **)malloc(sizeof(char *) * (Args.size() + 1));
		for (i = 0; i < Args.size(); ++i)
			argv[i] = Args[i].c_str();
		argv[i] = NULL;

		execvp(argv[0], (char *const *)argv);

		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		exit(1);
		/* NOTREACHED */
	}

	int wstatus;
	pid_t child2 = waitpid(child, &wstatus, 0);
	if (child2 < 0) {
		throw std::runtime_error(std::string(strerror(errno)));
		/* NOTREACHED */
	}
	if (WIFSIGNALED(wstatus)) {
		throw std::runtime_error(
			"Error: the command " + Args[0] + " terminated by signal " +
			std::to_string(WTERMSIG(wstatus)));
		/* NOTREACHED */
	}
	if (!WIFEXITED(wstatus)) {
		throw std::runtime_error(
			"Fatal: the command " + Args[0] + " does not exit");
		/* NOTREACHED */
	}
	if (0 != WEXITSTATUS(wstatus)) {
		std::stringstream errmsg;
		size_t i;

		errmsg << "bb::cmd, system command failed" << std::endl;

		errmsg << "Command line: ";
		for (i = 0; i < Args.size(); ++i) {
			errmsg << Args[i];
			if (i + 1 < Args.size())
				errmsg << " ";
		}
		errmsg << std::endl;

		errmsg << "Error code: " << std::to_string(WEXITSTATUS(wstatus)) <<
			std::endl;
		throw std::runtime_error(errmsg.str());
		/* NOTREACHED */
	}
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

