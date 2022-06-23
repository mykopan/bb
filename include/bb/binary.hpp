
/* -------------------------------------------------------------------------- */

#ifndef __BB_BINARY_HPP__
# define __BB_BINARY_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <istream>
# include <ostream>
# include <string>
# include <string_view>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

extern void encode(std::ostream&, std::uint8_t);
extern void decode(std::istream&, std::uint8_t&);

extern void encode(std::ostream&, std::uint16_t);
extern void decode(std::istream&, std::uint16_t&);

extern void encode(std::ostream&, std::uint32_t);
extern void decode(std::istream&, std::uint32_t&);

extern void encode(std::ostream&, std::uint64_t);
extern void decode(std::istream&, std::uint64_t&);

extern void encode(std::ostream&, std::int8_t);
extern void decode(std::istream&, std::int8_t&);

extern void encode(std::ostream&, std::int16_t);
extern void decode(std::istream&, std::int16_t&);

extern void encode(std::ostream&, std::int32_t);
extern void decode(std::istream&, std::int32_t&);

extern void encode(std::ostream&, std::int64_t);
extern void decode(std::istream&, std::int64_t&);

extern void encode(std::ostream&, std::size_t);
extern void decode(std::istream&, std::size_t&);

extern void encode(std::ostream&, const std::string&);
extern void encode(std::ostream&, const std::string_view&);
extern void decode(std::istream&, std::string&);

}

#endif /* !defined(__BB_BINARY_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

