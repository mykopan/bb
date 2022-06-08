
/* -------------------------------------------------------------------------- */

#ifndef __BB_TYPES_HPP__
# define __BB_TYPES_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <functional>
# include <string>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb {

/* The verbosity data type, used by options::verbosity. */
enum verbosity {
	/* Don't print any messages. */
	SILENT = 0,

	/* Only print error messages. */
	ERROR,

	/* Print errors and warnings. */
	WARN,

	/* Print errors, warnings and # command-name (for file-name) when running
	 * a traced command. */
	INFO,

	/* Print errors, warnings, full command lines when running a command
	 * or cmd command and status messages when starting a rule. */
	VERBOSE,

	/* Print messages for virtually everything (mostly for debugging). */
	DIAGNOSTIC
};


/* How should you determine if a file has changed, used by bb::options::change.
 * The most common values are MODTIME (the default, very fast,
 * touch causes files to rebuild) and MODTIME_AND_DIGEST_INPUT
 * (slightly slower, touch and switching git branches does not cause input files
 * to rebuild). */
enum change {
	/* Compare equality of modification timestamps, a file has changed if its
	 * last modified time changes. A touch will force a rebuild. This mode is
	 * fast and usually sufficiently accurate, so is the default. */
	MODTIME = 0,

	/* Compare equality of file contents digests, a file has changed if its
	 * digest changes. A touch will not force a rebuild. Use this mode if
	 * modification times on your file system are unreliable. */
	DIGEST,

	/* A file is rebuilt if both its modification time and digest have changed.
	 * For efficiency reasons, the modification time is checked first,
	 * and if that has changed, the digest is checked. */
	MODTIME_AND_DIGEST,

	/* Use change::MODTIME_AND_DIGEST for input/source files and
	 * change::MODTIME for output files. An input file is one which is a
	 * dependency but is not built by BB as it has no matching rule and already
	 * exists on the file system. */
	MODTIME_AND_DIGEST_INPUT,

	/* A file is rebuilt if either its modification time or its digest has
	 * changed. A touch will force a rebuild, but even if a files modification
	 * time is reset afterwards, changes will also cause a rebuild. */
	MODTIME_OR_DIGEST
};


/* Options to control the execution of BB. */
struct options {
	/* Defaults to "build". The directory used for storing BB metadata files.
	 * All metadata files will be named files/.bb.file-name, for some file-name.
	 * If the files directory does not exist it will be created.
	 * If set to "" then no files are read or written. */
	std::string files;

	/* Defaults to 1. Maximum number of rules to run in parallel,
	 * similar to make --jobs=N. For many build systems, a number equal to or
	 * slightly less than the number of physical processors works well.
	 * Use 0 to match the detected number of processors. */
	int nthreads;

	/* Defaults to "1". The version number of your build rules.
	 * Change the version number to force a complete rebuild, such as when
	 * making significant changes to the rules that require a wipe.
	 * The version number should be set in the source code, and not passed on
	 * the command line. */
	std::string version;

	/* Defaults to bb::INFO.
	 * What level of messages should be printed out. */
	verbosity verbosity;

	/* Defaults to false. Operate in staunch mode, where building continues even
	 * after errors, similar to make --keep-going. */
	bool staunch;

	/* Default to bb::MODTIME. How to check if a file has changed,
	 * see bb::change for details. */
	change change;

	/* Defaults to writing using std::cout << message << '\n'. A function
	 * called to output messages from BB, along with the verbosity at which that
	 * message should be printed. This function will be called atomically from
	 * all other output functions. */
	std::function<void(bb::verbosity, const std::string&)> output;
};


struct rules;    /* The set of build rules */
struct acontext; /* The build action context */

}

#endif /* !defined(__BB_TYPES_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

