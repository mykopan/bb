
/* -------------------------------------------------------------------------- */

#ifndef __BB_GENERAL_IDS_I_HPP__
# define __BB_GENERAL_IDS_I_HPP__

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

namespace bb::ids {

using intern::id;

template<typename V>
struct ids {
	ids()
		: values()
	{
	}

	size_t size_upper_bound() const
	{
		return values.size();
	}

	size_t size() const
	{
		size_t retval = 0;
		for (const auto& ov : values) {
			if (ov)
				++retval;
		}
		return retval;
	}

	bool empty() const
	{
		return 0 == size();
	}

	std::unordered_map<id, V> to_map() const
	{
		std::unordered_map<id, V> retval;
		for (id k = 0; k < values.size(); ++k) {
			if (values[k])
				retval[k] = *values[k];
		}
		return retval;
	}

	std::vector< std::pair<id, V> > to_vector() const
	{
		std::vector< std::pair<id, V> > retval;
		for (id k = 0; k < values.size(); ++k) {
			if (values[k])
				retval.emplace_back(k, *values[k]);
		}
		return retval;
	}

	void insert(id k, const V& v)
	{
		if (k <	values.size()) {
			values[k] = v;
		}
		else {
			for (id i = values.size(); i < k; ++i)
				values.push_back({});
			values.push_back(v);
		}
	}

	std::optional<V> lookup(id k) const
	{
		return k < values.size() ? values[k] : std::optional<V>{};
	}

	V& operator[] (id k)
	{
		assert(k < values.size());
		return *values[k];
	}

private:
	std::vector< std::optional<V> > values;
};

}

#endif /* !defined(__BB_GENERAL_IDS_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

