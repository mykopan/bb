
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/typesI.hpp"
#include "bb/errorsI.hpp"
#include "bb/rule.hpp"
#include "bb/rules/start.hpp"
#include "bb/rules/file.hpp"

#include <sstream>

#include <sys/stat.h>

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
			return aFileQ.file_path.string();
		},
		.hash = [](const fileQ& aFileQ) {
			return std::hash<std::string>{}(aFileQ.file_path);
		},
		.equal = [](const fileQ& aLhs, const fileQ& aRhs) {
			return aLhs.file_path == aRhs.file_path;
		},
		.encode = [](std::ostream& aStrm, const fileQ& aFileQ) {
			encode(aStrm, aFileQ.file_path.string());
		},
		.decode = [](std::istream& aStrm) {
			std::string pth;
			decode(aStrm, pth);
			return fileQ(pth);
		}
	};
	return cls;
}

struct fileA {
	fileA(size_t aSize, time_t aModTime)
		: size(aSize), mtime(aModTime)
	{}

	size_t size;
	time_t mtime;
};

static void encode(std::ostream& s, time_t x)
{
	encode(s, static_cast<std::uint64_t>(x));
}

static void decode(std::istream& s, time_t& x)
{
	std::uint64_t y;
	decode(s, y);
	x = static_cast<time_t>(y);
}

static const object_cls<fileA>& object_cls_instance(proxy<fileA>)
{
	static const object_cls<fileA> cls = {
		.show = [](const fileA& aFileA) {
			std::stringstream output;
			output << "fileA{.size=" << aFileA.size <<
				", .mtime=" << aFileA.mtime << "}";
			return output.str();
		},
		.hash = [](const fileA& aFileA) {
			return std::hash<size_t>{}(aFileA.size)
				^ std::hash<time_t>{}(aFileA.mtime);
		},
		.equal = [](const fileA& aLhs, const fileA& aRhs) {
			return aLhs.size == aRhs.size
				&& aLhs.mtime == aRhs.mtime;
		},
		.encode = [](std::ostream& aStrm, const fileA& aFileA) {
			encode(aStrm, aFileA.size);
			encode(aStrm, aFileA.mtime);
		},
		.decode = [](std::istream& aStrm) {
			size_t size;
			time_t mtime;
			decode(aStrm, size);
			decode(aStrm, mtime);
			return fileA(size, mtime);
		}
	};
	return cls;
}

static fileA _t_stored_value(const fileQ& aFileQ)
{
	struct stat sbuf;

	if (lstat(aFileQ.file_path.string().c_str(), &sbuf) < 0) {
		std::error_code ecode;
		switch (errno) {
		case EACCES:
			ecode = std::make_error_code(std::errc::permission_denied);
			break;
		case EFAULT:
			ecode = std::make_error_code(std::errc::bad_address);
			break;
		case EIO:
			ecode = std::make_error_code(std::errc::io_error);
			break;
		case ELOOP:
			ecode = std::make_error_code(std::errc::too_many_symbolic_link_levels);
			break;
		case ENAMETOOLONG:
			ecode = std::make_error_code(std::errc::filename_too_long);
			break;
		case ENOENT:
			ecode = std::make_error_code(std::errc::no_such_file_or_directory);
			break;
		case ENOMEM:
			ecode = std::make_error_code(std::errc::not_enough_memory);
			break;
		case ENOTDIR:
			ecode = std::make_error_code(std::errc::not_a_directory);
			break;
		case EOVERFLOW:
			ecode = std::make_error_code(std::errc::value_too_large);
			break;
		default:
			assert(0 && "not reachable");
		}
		throw fs::filesystem_error(ecode.message(),
			aFileQ.file_path, ecode);
		/* NOTREACHED */
	}

	if (S_ISDIR(sbuf.st_mode)) {
		throw error_directory_not_file(aFileQ.file_path);
		/* NOTREACHED */
	}
	if (!S_ISREG(sbuf.st_mode) && !S_ISLNK(sbuf.st_mode)) {
		throw error_not_file(aFileQ.file_path);
		/* NOTREACHED */
	}

	return fileA(sbuf.st_size, sbuf.st_mtime);
}

static const rule_cls<fileQ, fileA>&
rule_cls_instance(proxy<fileQ> k, proxy<fileA> v)
{
	static const rule_cls<fileQ, fileA> cls = {
		.key_cls = object_cls_instance(k),
		.value_cls = object_cls_instance(v),
		.stored = [](const fileQ& aFileQ) {
			try {
				return std::make_optional(_t_stored_value(aFileQ));
			}
			catch (...) {
				return std::optional<fileA>{};
			}
		}
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

