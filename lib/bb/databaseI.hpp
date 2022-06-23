
/* -------------------------------------------------------------------------- */

#ifndef __BB_DATABASE_I_HPP__
# define __BB_DATABASE_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/dynobjI.hpp"
# include "bb/general/internI.hpp"
# include "bb/general/idsI.hpp"
# include "bb/general/stackI.hpp"
# include "bb/general/variantI.hpp"

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb::db {

using intern::id;
using intern::intern;
using ids::ids;
using stack = stack::stack<dynamic_key>;
using depends = std::vector<id>;

typedef std::uint32_t time;

struct result {
	dynamic_value    value;
	time             built;
	time             changed;
	depends          depends;
};

struct loaded { result loaded_result; };
struct missing {};
struct waiting {};
struct ready { result ready_result; };
struct error { std::exception_ptr eptr; };
using status = std::variant<loaded, missing, waiting, ready, error>;

struct key_status {
	dynamic_key key;
	status      status;
};

using intern_db = intern<dynamic_key>;
using status_db = ids<key_status>;

struct database {
	time        now;
	intern_db   intern;
	status_db   status;
};

struct build_ops {
	std::function<bool(const dynamic_key&, const dynamic_value&)> valid;
	std::function<std::pair<dynamic_value, depends>(const stack&, const dynamic_key&)> build;
};

extern std::pair<std::vector<dynamic_value>, depends>
build(
	database&,
	const build_ops&,
	const stack&,
	const std::vector<dynamic_key>&
	);

}

#endif /* !defined(__BB_DATABASE_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

