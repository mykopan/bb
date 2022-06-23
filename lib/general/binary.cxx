
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/binary.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

void encode(std::ostream& s, std::uint8_t x)
{
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::uint8_t& x)
{
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::uint16_t x)
{
	// @todo: big endian
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::uint16_t& x)
{
	// @todo: big endian
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::uint32_t x)
{
	// @todo: big endian
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::uint32_t& x)
{
	// @todo: big endian
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::uint64_t x)
{
	// @todo: big endian
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::uint64_t& x)
{
	// @todo: big endian
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::int8_t x)
{
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::int8_t& x)
{
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::int16_t x)
{
	// @todo: big endian
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::int16_t& x)
{
	// @todo: big endian
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::int32_t x)
{
	// @todo: big endian
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::int32_t& x)
{
	// @todo: big endian
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::int64_t x)
{
	// @todo: big endian
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::int64_t& x)
{
	// @todo: big endian
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, std::size_t x)
{
	// @todo: big endian
	s.write(reinterpret_cast<const char*>(&x), sizeof x);
}

void decode(std::istream& s, std::size_t& x)
{
	// @todo: big endian
	s.read(reinterpret_cast<char*>(&x), sizeof x);
}

void encode(std::ostream& s, const std::string& x)
{
	encode(s, x.size());
	s.write(x.data(), x.size());
}

void encode(std::ostream& s, const std::string_view& x)
{
	encode(s, x.size());
	s.write(x.data(), x.size());
}

void decode(std::istream& s, std::string& x)
{
	std::size_t sz;
	decode(s, sz);
	x.clear();
	x.resize(sz);
	s.read(x.data(), sz);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

