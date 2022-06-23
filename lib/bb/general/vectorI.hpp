
/* -------------------------------------------------------------------------- */

#ifndef __BB_GENERAL_VECTOR_I_HPP__
# define __BB_GENERAL_VECTOR_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <vector>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

template<typename T>
void
append(
	std::vector<T>& aSrcDst,
	const std::vector<T>& aSrc
	)
{
	aSrcDst.insert(aSrcDst.end(), aSrc.begin(), aSrc.end());
}

}

#endif /* !defined(__BB_GENERAL_VECTOR_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

