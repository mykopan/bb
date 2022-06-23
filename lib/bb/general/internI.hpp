
/* -------------------------------------------------------------------------- */

#ifndef __BB_GENERAL_INTERN_I_HPP__
# define __BB_GENERAL_INTERN_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <optional>
# include <unordered_map>
# include <utility>
# include <vector>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb::intern {

typedef std::uint32_t id;

template<typename K>
struct intern {
	intern()
		: free_id(0), map()
	{
	}

	intern(const std::vector< std::pair<K, id> >& aVector)
		: free_id(0), map()
	{
		for (const auto& kv : aVector) {
			free_id = std::max(free_id, kv.second + 1);
			map[kv.first] = kv.second;
		}
	}

	void insert(const K& k, id v)
	{
		free_id = std::max(free_id, v + 1);
		map[k] = v;
	}

	id add(const K& k)
	{
		id v = free_id++;
		map[k] = v;
		return v;
	}

	std::optional<id> lookup(const K& k) const
	{
		auto iter = map.find(k);
		return iter != map.end() ? iter->second : std::optional<id>{};
	}

	std::vector< std::pair<K, id> > to_vector() const
	{
		std::vector< std::pair<K, id> > retval;

		retval.reserve(map.size());
		for (const auto& kv : map)
			retval.emplace_back(kv.first, kv.second);
		return retval;
	}

private:
	id                        free_id;
	std::unordered_map<K, id> map;
};

}

#endif /* !defined(__BB_GENERAL_INTERN_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

