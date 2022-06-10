
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/errorsI.hpp"
#include "bb/rule.hpp"
#include "bb/rules/start.hpp"
#include "bb/rules/file.hpp"
#include <sstream>
#include <iomanip>

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

namespace fs = std::filesystem;

struct fileQ {
	fileQ(const filepath& x) : file_path(x) {}
	filepath file_path;
};

static const object_cls<fileQ> _S_fileQ_cls(
	[](const object<fileQ>& aFileQ) {
		std::stringstream output;
		output << "fileQ(" << std::quoted(aFileQ->file_path.string()) << ")";
		return (output.str());
	},
	[](const object<fileQ>& aFileQ) {
		return std::hash<std::string>{}(aFileQ->file_path);
	},
	[](const object<fileQ>& aLhs, const object<fileQ>& aRhs) {
		return (aLhs->file_path == aRhs->file_path);
	}
);

struct fileA {
	fileA(fs::file_time_type x) : mtime(x) {}
	fs::file_time_type mtime;
};

static const object_cls<fileA> _S_fileA_cls(
	[](const object<fileA>& aFileA) {
		(void) aFileA;
		std::stringstream output;
		output << "fileA(@todo-mtime)";
		return (output.str());
	},
	[](const object<fileA>& aFileA) {
		(void) aFileA;
		// return std::hash<fs::file_time_type>{}(fileValue->mtime);
		return 0;
	},
	[](const object<fileA>& aLhs, const object<fileA>& aRhs) {
		return (aLhs->mtime == aRhs->mtime);
	}
);

static const rule_cls<fileQ, fileA> _S_file_rule_cls = {
	.key_cls = _S_fileQ_cls,
	.value_cls = _S_fileA_cls
};

void want(rules& Rules, const std::vector<filepath>& Files)
{
	action(Rules, [Files](acontext& aCtx) { need(aCtx, Files); });
}

void need(acontext& aCtx, const std::vector<filepath>& Files)
{
	std::vector< key<fileQ> > qFiles;
	for (const auto& file : Files)
		qFiles.push_back(std::make_shared<fileQ>(file));
	apply_rules_(aCtx, _S_file_rule_cls, qFiles);
}

static value<fileA> _t_stored_value(const key<fileQ>& aFileQ)
{
	fs::file_status stat = symlink_status(aFileQ->file_path);
	if (fs::exists(stat)) {
		if (fs::is_directory(stat)) {
			throw error_directory_not_file(aFileQ->file_path);
			/* NOTREACHED */
		}
		if (fs::is_symlink(stat)) {
			throw error_symlink_not_file(aFileQ->file_path);
			/* NOTREACHED */
		}
		if (!fs::is_regular_file(stat)) {
			throw error_not_file(aFileQ->file_path);
			/* NOTREACHED */
		}
	}
	return std::make_shared<fileA>(fs::last_write_time(aFileQ->file_path));
}

void default_file_rule(rules& Rules)
{
	add_rule<fileQ, fileA>(Rules, _S_file_rule_cls, 0,
		[](const key<fileQ>& aFileQ) { (void)aFileQ; return (true); },
		[](acontext& aCtx, const key<fileQ>& aFileQ) {
			value<fileA> fileA;

			(void)aCtx;

			try {
				fileA = _t_stored_value(aFileQ);
			}
			catch (const fs::filesystem_error& err) {
				if (err.code() == std::errc::no_such_file_or_directory) {
					throw std::runtime_error(
						"Error: file does not exist and no rule available: " +
						aFileQ->file_path.string());
				}
				throw;
			}
			return (fileA);
		});
}

void
file(
	rules& Rules,
	const fpattern& aPattern,
	const std::function<void(acontext&, const filepath&)>& anAction
	)
{
	add_rule<fileQ, fileA>(Rules, _S_file_rule_cls, 1,
		[aPattern](const key<fileQ>& aFileQ) {
			return is_subset_fpattern(aFileQ->file_path, aPattern);
		},
		[anAction](acontext& aCtx, const key<fileQ>& aFileQ) {
			value<fileA> fileA;

			fs::create_directories(aFileQ->file_path.parent_path());
			anAction(aCtx, aFileQ->file_path);
			try {
				fileA = _t_stored_value(aFileQ);
			}
			catch (const fs::filesystem_error& err) {
				if (err.code() == std::errc::no_such_file_or_directory) {
					throw std::runtime_error(
						"Error, rule failed to build the file: " +
						aFileQ->file_path.string());
				}
				throw;
			}
			return (fileA);
		});
}

static bool is_subset(const char* aPath, char const* aPattern)
{
	for (;;) {
		switch (*aPattern) {
		case '\0':
			return '\0' == *aPath;
		case '*': // (* -> x* | x != PATH_SEP) || (* ->)
			return ('\0' != *aPath && '/' != *aPath && '\\' != *aPath
					&& is_subset(aPath + 1, aPattern))
				|| is_subset(aPath, aPattern + 1);
		case '/':
			if ('/' != *aPath && '\\' != *aPath)
				return false;
			++aPath, ++aPattern;
			break;
		default:
			if (*aPath != *aPattern)
				return false;
			++aPath, ++aPattern;
			break;
		}
	}
}

bool is_subset_fpattern(const filepath& aPath, const fpattern& aPattern)
{
	return is_subset(aPath.c_str(), aPattern.c_str());
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

