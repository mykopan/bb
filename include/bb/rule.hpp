
/* -------------------------------------------------------------------------- */

#ifndef __BB_RULE_HPP__
# define __BB_RULE_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>
# include <functional>
# include <memory>

/*******************************************************************************
 * %% BeginSection: function declarations
 */
# pragma mark -
# pragma mark [ function declarations ]
# pragma mark -

namespace bb {

typedef std::shared_ptr<void> object;
typedef object                key;
typedef object                value;

struct object_cls {
	const std::type_info&                             type_info;
	std::function<std::string(const object&)>         show;
	std::function<std::size_t(const object&)>         hash;
	std::function<bool(const object&, const object&)> equal;
};

struct rule_cls {
	const object_cls&  key_cls;
	const object_cls&  value_cls;
};

extern void
add_rule(
	rules&,
	const rule_cls&,
	int,
	const std::function<bool(const key&)>&,
	const std::function<value(acontext&, const key&)>&
	);

extern std::vector<value>
apply_rules(acontext&, const rule_cls&, const std::vector<key>&);

extern void apply_rules_(acontext&, const rule_cls&, const std::vector<key>&);

extern value apply_rule(acontext&, const rule_cls&, const key&);
extern void apply_rule_(acontext&, const rule_cls&, const key&);

}

#endif /* !defined(__BB_RULE_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

