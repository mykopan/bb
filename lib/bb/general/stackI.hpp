
/* -------------------------------------------------------------------------- */

#ifndef __BB_GENERAL_STACK_I_HPP__
# define __BB_GENERAL_STACK_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include "bb/general/internI.hpp"
# include "bb/general/purelistI.hpp"
# include "bb/general/vectorI.hpp"

# include <optional>
# include <string>
# include <utility>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb::stack {

using intern::id;

template<typename K>
struct stack {
	stack()
		: track()
	{}

	std::optional<K> check(id i) const
	{
		auto mbElm = purelist::find< std::pair<id, K> >(track,
			[i] (const std::pair<id, K>& p) { return i == p.first; });
		if (mbElm) {
			return mbElm->second;
			/* NOTREACHED */
		}
		return {};
	}

	std::optional< std::pair<id, K> > check(const std::vector<id>& ids) const
	{
		for (id i : ids) {
			std::optional<K> badKey = check(i);
			if (badKey) {
				return std::pair<id, K>(i, *badKey);
				/* NOTREACHED */
			}
		}
		return {};
	}

	std::vector<std::string> show() const
	{
		std::vector<std::string> retval;
		for (auto xs = track; xs; xs = xs->tail)
			retval.push_back(xs->head.second.show());
		std::reverse(retval.begin(), retval.end());
		return retval;
	}

	stack<K> push(id i, const K& k) const
	{
		stack<K> retval;
		retval.track = purelist::cons(std::pair<id, K>(i, k), track);
		return retval;
	}

private:
	purelist::list< std::pair<id, K> > track;
	/* @todo: design pure int set for fast check */
};

}

#endif /* !defined(__BB_GENERAL_STACK_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

