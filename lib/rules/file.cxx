
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

static const object_cls<fileQ>& object_cls_instance(proxy<fileQ>)
{
	static const object_cls<fileQ> cls = {
		.show = [](const fileQ& aFileQ) {
			std::stringstream output;
			output << "fileQ(" << std::quoted(aFileQ.file_path.string()) << ")";
			return (output.str());
		},
		.hash = [](const fileQ& aFileQ) {
			return std::hash<std::string>{}(aFileQ.file_path);
		},
		.equal = [](const fileQ& aLhs, const fileQ& aRhs) {
			return (aLhs.file_path == aRhs.file_path);
		}
	};
	return cls;
}

struct fileA {
	fileA(fs::file_time_type x) : mtime(x) {}
	fs::file_time_type mtime;
};

static const object_cls<fileA>& object_cls_instance(proxy<fileA>)
{
	static const object_cls<fileA> cls = {
		.show = [](const fileA& aFileA) {
			(void) aFileA;
			std::stringstream output;
			output << "fileA(@todo-mtime)";
			return (output.str());
		},
		.hash = [](const fileA& aFileA) {
			(void) aFileA;
			// return std::hash<fs::file_time_type>{}(fileValue->mtime);
			return 0;
		},
		.equal = [](const fileA& aLhs, const fileA& aRhs) {
			return (aLhs.mtime == aRhs.mtime);
		}
	};
	return cls;
}

static const rule_cls<fileQ, fileA>&
rule_cls_instance(proxy<fileQ> k, proxy<fileA> v)
{
	static const rule_cls<fileQ, fileA> cls = {
		.key_cls = object_cls_instance(k),
		.value_cls = object_cls_instance(v)
	};
	return cls;
}

void want(rules& Rules, const std::vector<filepath>& Files)
{
	action(Rules, [Files](acontext& aCtx) { need(aCtx, Files); });
}

void need(acontext& aCtx, const std::vector<filepath>& Files)
{
	std::vector<fileQ> qFiles;
	for (const auto& file : Files)
		qFiles.push_back(fileQ(file));
	apply_rules_<fileQ, fileA>(aCtx, qFiles);
}

static fileA _t_stored_value(const fileQ& aFileQ)
{
	fs::file_status stat = symlink_status(aFileQ.file_path);
	if (fs::exists(stat)) {
		if (fs::is_directory(stat)) {
			throw error_directory_not_file(aFileQ.file_path);
			/* NOTREACHED */
		}
		if (fs::is_symlink(stat)) {
			throw error_symlink_not_file(aFileQ.file_path);
			/* NOTREACHED */
		}
		if (!fs::is_regular_file(stat)) {
			throw error_not_file(aFileQ.file_path);
			/* NOTREACHED */
		}
	}
	return fileA(fs::last_write_time(aFileQ.file_path));
}

void default_file_rule(rules& Rules)
{
	add_rule<fileQ, fileA>(Rules, 0,
		[](const fileQ&) { return true; },
		[](acontext&, const fileQ& aFileQ) {
			try {
				return _t_stored_value(aFileQ);
				/* NOTREACHED */
			}
			catch (const fs::filesystem_error& err) {
				if (err.code() == std::errc::no_such_file_or_directory) {
					throw std::runtime_error(
						"Error: file does not exist and no rule available: " +
						aFileQ.file_path.string());
					/* NOTREACHED */
				}
				throw;
				/* NOTREACHED */
			}
		});
}

void
file(
	rules& Rules,
	const fpattern& aPattern,
	const std::function<void(acontext&, const filepath&)>& anAction
	)
{
	add_rule<fileQ, fileA>(Rules, 1,
		[aPattern](const fileQ& aFileQ) {
			return is_subset_fpattern(aFileQ.file_path, aPattern);
		},
		[anAction](acontext& aCtx, const fileQ& aFileQ) {
			fs::create_directories(aFileQ.file_path.parent_path());
			anAction(aCtx, aFileQ.file_path);
			try {
				return _t_stored_value(aFileQ);
				/* NOTREACHED */
			}
			catch (const fs::filesystem_error& err) {
				if (err.code() == std::errc::no_such_file_or_directory) {
					throw std::runtime_error(
						"Error, rule failed to build the file: " +
						aFileQ.file_path.string());
					/* NOTREACHED */
				}
				throw;
				/* NOTREACHED */
			}
		});
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

