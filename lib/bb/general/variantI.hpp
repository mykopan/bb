
/* -------------------------------------------------------------------------- */

#ifndef __BB_GENERAL_VARIANT_I_HPP__
# define __BB_GENERAL_VARIANT_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <variant>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}

#endif /* !defined(__BB_GENERAL_VARIANT_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

