
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/errorsI.hpp"
#include "bb/rule.hpp"
#include "bb/rules/start.hpp"
#include "bb/rules/file.hpp"
// #include <memory>
#include <sstream>
#include <iomanip>

/*******************************************************************************
 * %% BeginSection: function definitions
 */
# pragma mark -
# pragma mark [ function definitions ]
# pragma mark -

namespace bb {

namespace fs = std::filesystem;

struct fileQ {
	fileQ(const filepath& x) : file_path(x) {}
	filepath file_path;
};

static const object_cls _S_fileQ_cls = {
	.type_info = typeid(fileQ),
	.show = [](const object& aFileQ) {
		const auto& fileKey = reinterpret_cast< const std::shared_ptr<fileQ>& >(aFileQ);
		std::stringstream output;
		output << "fileQ(" << std::quoted(fileKey->file_path.string()) << ")";
		return (output.str());
	},
	.hash = [](const object& aFileQ) {
		const auto& fileKey = reinterpret_cast< const std::shared_ptr<fileQ>& >(aFileQ);
		return std::hash<std::string>{}(fileKey->file_path);
	},
	.equal = [](const object& aLhsQ, const object& aRhsQ) {
		const auto& lhsKey = reinterpret_cast< const std::shared_ptr<fileQ>& >(aLhsQ);
		const auto& rhsKey = reinterpret_cast< const std::shared_ptr<fileQ>& >(aRhsQ);
		return (lhsKey->file_path == rhsKey->file_path);
	}
};

struct fileA {
	fileA(fs::file_time_type x) : mtime(x) {}
	fs::file_time_type mtime;
};

static const object_cls _S_fileA_cls = {
	.type_info = typeid(fileA),
	.show = [](const object& aFileA) {
		const auto& fileValue = reinterpret_cast< const std::shared_ptr<fileA>& >(aFileA);
		(void) fileValue;
		std::stringstream output;
		output << "fileA(@todo-mtime)";
		return (output.str());
	},
	.hash = [](const object& aFileA) {
		const auto& fileValue = reinterpret_cast< const std::shared_ptr<fileA>& >(aFileA);
		(void) fileValue;
		// return std::hash<fs::file_time_type>{}(fileValue->mtime);
		return 0;
	},
	.equal = [](const object& aLhsA, const object& aRhsA) {
		const auto& lhsValue = reinterpret_cast< const std::shared_ptr<fileA>& >(aLhsA);
		const auto& rhsValue = reinterpret_cast< const std::shared_ptr<fileA>& >(aRhsA);
		return (lhsValue->mtime == rhsValue->mtime);
	}
};

static const rule_cls _S_file_rule_cls = {
	.key_cls = _S_fileQ_cls,
	.value_cls = _S_fileA_cls
};

void want(rules& Rules, const std::vector<filepath>& Files)
{
	action(Rules, [Files](acontext& aCtx) { need(aCtx, Files); });
}

void need(acontext& aCtx, const std::vector<filepath>& Files)
{
	std::vector<key> qFiles;
	for (const auto& file : Files)
		qFiles.push_back(std::make_shared<fileQ>(file));
	apply_rules_(aCtx, _S_file_rule_cls, qFiles);
}

static std::shared_ptr<fileA>
_t_stored_value(const std::shared_ptr<fileQ>& aFileQ)
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
	add_rule(Rules, _S_file_rule_cls, 0,
		[](const key& aFileQ) { (void)aFileQ; return (true); },
		[](acontext& aCtx, const key& aFileQ) {
			const auto& fileKey = reinterpret_cast< const std::shared_ptr<fileQ>& >(aFileQ);
			std::shared_ptr<fileA> fileValue;

			(void)aCtx;

			try {
				fileValue = _t_stored_value(fileKey);
			}
			catch (const fs::filesystem_error& err) {
				if (err.code() == std::errc::no_such_file_or_directory) {
					throw std::runtime_error(
						"Error: file does not exist and no rule available: " +
						fileKey->file_path.string());
				}
				throw;
			}
			return static_cast<value>(fileValue);
		});
}

void
file(
	rules& Rules,
	const fpattern& aPattern,
	const std::function<void(acontext&, const filepath&)>& anAction
	)
{
	add_rule(Rules, _S_file_rule_cls, 1,
		[aPattern](const key& aFileQ) {
			const auto& fileKey = reinterpret_cast< const std::shared_ptr<fileQ>& >(aFileQ);
			return is_subset_fpattern(fileKey->file_path, aPattern);
		},
		[anAction](acontext& aCtx, const key& aFileQ) {
			const auto& fileKey = reinterpret_cast< const std::shared_ptr<fileQ>& >(aFileQ);
			std::shared_ptr<fileA> fileValue;

			fs::create_directories(fileKey->file_path.parent_path());
			anAction(aCtx, fileKey->file_path);
			try {
				fileValue = _t_stored_value(fileKey);
			}
			catch (const fs::filesystem_error& err) {
				if (err.code() == std::errc::no_such_file_or_directory) {
					throw std::runtime_error(
						"Error, rule failed to build the file: " +
						fileKey->file_path.string());
				}
				throw;
			}
			return static_cast<value>(fileValue);
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

