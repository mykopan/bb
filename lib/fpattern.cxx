
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/rules/file.hpp"
#include <optional>
#include <string_view>
#include <variant>

// #define _DEBUG_FPATTERN

#ifdef _DEBUG_FPATTERN
# include <iostream>
#endif

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb {

struct pat_lit { std::string_view lit; };
struct pat_star {};
struct pat_skip {};
struct pat_skip1 {};
struct pat_stars {
	std::string_view prefix;
	std::vector<std::string_view> infix;
	std::string_view suffix;
};
using pat = std::variant<pat_lit, pat_star, pat_skip, pat_skip1, pat_stars>;

#ifdef _DEBUG_FPATTERN
static std::ostream& operator<<(std::ostream& os, const pat_lit& p)
{
	return os << "pat_lit{" << std::quoted(p.lit) << "}";
}

static std::ostream& operator<<(std::ostream& os, const pat_star&)
{
	return os << "pat_star";
}

static std::ostream& operator<<(std::ostream& os, const pat_skip&)
{
	return os << "pat_skip";
}

static std::ostream& operator<<(std::ostream& os, const pat_skip1&)
{
	return os << "pat_skip1";
}

static std::ostream& operator<<(std::ostream& os, const pat_stars& p)
{
	os << "pat_stars{prefix=" << std::quoted(p.prefix) << ",infix={";
	bool first = true;
	for (const auto& infix : p.infix) {
		if (!first)
			os << ",";
		os << std::quoted(infix);
		first = false;
	}
	return os << "},suffix=" << std::quoted(p.suffix);
}

static std::ostream& operator<<(std::ostream& os, const pat& p)
{
	if (std::holds_alternative<pat_lit>(p)) {
		return os << std::get<pat_lit>(p);
		/* NOTREACHED */
	}
	if (std::holds_alternative<pat_star>(p)) {
		return os << std::get<pat_star>(p);
		/* NOTREACHED */
	}
	if (std::holds_alternative<pat_skip>(p)) {
		return os << std::get<pat_skip>(p);
		/* NOTREACHED */
	}
	if (std::holds_alternative<pat_skip1>(p)) {
		return os << std::get<pat_skip1>(p);
		/* NOTREACHED */
	}
	if (std::holds_alternative<pat_stars>(p)) {
		return os << std::get<pat_stars>(p);
		/* NOTREACHED */
	}
	throw std::runtime_error("unreachable");
}

static std::ostream& operator<<(std::ostream& os, const std::vector<pat>& pats)
{
	os << "{";
	bool first = true;
	for (const auto& p : pats) {
		if (!first)
			os << ",";
		os << p;
		first = false;
	}
	return os << "}";
}
#endif // _DEBUG_FPATTERN


struct lex_str { std::string_view str; };
struct lex_slash {};
struct lex_slash_slash {};
using lexeme = std::variant<lex_str, lex_slash, lex_slash_slash>;

#ifdef _DEBUG_FPATTERN
static std::ostream& operator<<(std::ostream& os, const lex_str& l)
{
	return os << "lex_str{" << std::quoted(l.str) << "}";
}

static std::ostream& operator<<(std::ostream& os, const lex_slash&)
{
	return os << "lex_slash";
}

static std::ostream& operator<<(std::ostream& os, const lex_slash_slash&)
{
	return os << "lex_slash_slash";
}

static std::ostream& operator<<(std::ostream& os, const lexeme& l)
{
	if (std::holds_alternative<lex_str>(l)) {
		return os << std::get<lex_str>(l);
		/* NOTREACHED */
	}
	if (std::holds_alternative<lex_slash>(l)) {
		return os << std::get<lex_slash>(l);
		/* NOTREACHED */
	}
	if (std::holds_alternative<lex_slash_slash>(l)) {
		return os << std::get<lex_slash_slash>(l);
		/* NOTREACHED */
	}
	throw std::runtime_error("unreachable");
}

static std::ostream& operator<<(std::ostream& os, const std::vector<lexeme>& toks)
{
	os << "{";
	bool first = true;
	for (const auto& tok : toks) {
		if (!first)
			os << ",";
		os << tok;
		first = false;
	}
	return os << "}";
}
#endif // _DEBUG_FPATTERN


static inline bool is_path_sep(char c)
{
	return '/' == c || '\\' == c;
}

static inline bool is_relative_pattern(const fpattern& aPattern)
{
	if (2 <= aPattern.size() && '*' == aPattern[0] && '*' == aPattern[1]) {
		return aPattern.size() == 2 || is_path_sep(aPattern[2]);
		/* NOTREACHED */
	}
	return false;
}

static std::vector<lexeme> tokens(const std::string_view& aPattern)
{
	std::vector<lexeme> retval;
	size_t i, end;

	end = aPattern.size();
	i = 0;
	while (i < end) {
		if (is_path_sep(aPattern[i])) {
			if (i + 1 < end && is_path_sep(aPattern[i + 1])) {
				retval.push_back(lex_slash_slash{});
				i += 2;
				continue;
				/* NOTREACHED */
			}
			retval.push_back(lex_slash{});
			++i;
			continue;
			/* NOTREACHED */
		}

		size_t j = i;
		for (; j < end && !is_path_sep(aPattern[j]); ++j);
		retval.push_back(lex_str{ aPattern.substr(i, j - i) });
		i = j;
	}

	return retval;
}

static pat parse_lit(const std::string_view& aStr)
{
	if (aStr == "*") {
		return pat_star{};
		/* NOTREACHED */
	}

	size_t i = aStr.find('*');
	if (std::string_view::npos == i) {
		return pat_lit{ aStr };
		/* NOTREACHED */
	}

	pat_stars retval;

	retval.prefix = aStr.substr(0, i);

	std::string_view str;
	for (str = aStr.substr(i + 1);
		(i = str.find('*'), std::string_view::npos != i);
			str = str.substr(i + i))
	{
		retval.infix.push_back(str.substr(0, i));
	}

	retval.suffix = str;

	return retval;
}

static std::vector<pat> parse(const std::string_view& aPattern)
{
	std::vector<pat> retval;
	std::vector<lexeme> toks = tokens(aPattern);

	bool str = false;
	bool slash = true;

	size_t i = 0;
	size_t end = toks.size();
	while (i < end) {
		const lexeme& tok = toks[i];
		if (std::holds_alternative<lex_str>(tok)) {
			const std::string_view& lstr = std::get<lex_str>(tok).str;
			if ("**" == lstr)
				retval.push_back(pat_skip{});
			else
				retval.push_back(parse_lit(lstr));
			str = true;
			slash = false;
			++i;
			continue;
			/* NOTREACHED */
		}
		if (std::holds_alternative<lex_slash_slash>(tok)) {
			if (!str && i + 1 < end
				&& std::holds_alternative<lex_slash>(toks[i + 1]))
			{
				retval.push_back(pat_skip1{});
				slash = true;
				i += 2;
				continue;
				/* NOTREACHED */
			}
			retval.push_back(pat_skip{});
			slash = false;
			++i;
			continue;
			/* NOTREACHED */
		}
		assert(std::holds_alternative<lex_slash>(tok));
		if (!str)
			retval.push_back(pat_lit{ std::string_view() });
		slash = true;
		++i;
	}

	if (slash)
		retval.push_back(pat_lit{ std::string_view() });

	return retval;
}

static std::vector<pat> optimise(const std::vector<pat>& Pats)
{
	std::vector<pat> retval;
	size_t i, end;
	pat head;

	if (Pats.empty()) {
		return Pats;
		/* NOTREACHED */
	}

	head = Pats[0];
	end = Pats.size();
	for (i = 1; i < end; ) {
		if (std::holds_alternative<pat_skip>(head)) {
			if (std::holds_alternative<pat_skip>(Pats[i])) {
				++i;
				continue;
				/* NOTREACHED */
			}
			if (std::holds_alternative<pat_star>(Pats[i])) {
				head = pat_skip1{};
				++i;
				continue;
				/* NOTREACHED */
			}
		}
		if (std::holds_alternative<pat_star>(head)
			&& std::holds_alternative<pat_skip>(Pats[i]))
		{
			head = pat_skip1{};
			++i;
			continue;
			/* NOTREACHED */
		}

		retval.push_back(head);
		head = Pats[i];
		++i;
	}

	retval.push_back(head);
	return retval;
}

static std::optional<std::string_view>
strip_prefix(
	const std::string_view& aPrefix,
	const std::string_view& aStr
	)
{
	if (aStr.size() < aPrefix.size()) {
		return {};
		/* NOTREACHED */
	}

	std::string_view requiredPrefix = aStr.substr(0, aPrefix.size());
	if (requiredPrefix != aPrefix) {
		return {};
		/* NOTREACHED */
	}

	std::string_view retval(aStr);
	retval.remove_prefix(aPrefix.size());
	return retval;
}

static std::optional<std::string_view>
strip_suffix(
	const std::string_view& aSuffix,
	const std::string_view& aStr
	)
{
	if (aStr.size() < aSuffix.size()) {
		return {};
		/* NOTREACHED */
	}

	std::string_view requiredSuffix = aStr.substr(aStr.size() - aSuffix.size(),
		aSuffix.size());
	if (requiredSuffix != aSuffix) {
		return {};
		/* NOTREACHED */
	}

	std::string_view retval(aStr);
	retval.remove_suffix(aSuffix.size());
	return retval;
}

static std::optional< std::pair<std::string_view, std::string_view> >
strip_infix(
	const std::string_view& anInfix,
	const std::string_view& aStr
	)
{
	if (aStr.size() < anInfix.size()) {
		return {};
		/* NOTREACHED */
	}

	const char* pos = static_cast<const char *>(memmem(aStr.data(), aStr.size(),
		anInfix.data(), anInfix.size()));
	if (NULL == pos) {
		return {};
		/* NOTREACHED */
	}

	std::string_view before(aStr.data(),
		static_cast<size_t>(pos - aStr.data()));
	std::string_view after(pos + anInfix.size(),
		aStr.size() - anInfix.size() - before.size());
	return std::pair<std::string_view, std::string_view>{before, after};
}

static bool match_stars(std::string_view anElem, const pat_stars& Stars)
{
	auto elem = strip_prefix(Stars.prefix, anElem);
	if (!elem) {
		return false;
		/* NOTREACHED */
	}
	anElem = *elem;

	elem = strip_suffix(Stars.suffix, anElem);
	if (!elem) {
		return false;
		/* NOTREACHED */
	}
	anElem = *elem;

	for (const auto& infix : Stars.infix) {
		auto pair = strip_infix(infix, anElem);
		if (!pair) {
			return false;
			/* NOTREACHED */
		}
		anElem = pair->second;
	}

	return true;
}

static bool
match_impl(
	std::vector<std::string_view>::const_iterator anElem,
	std::vector<std::string_view>::const_iterator anElemEnd,
	std::vector<pat>::const_iterator aPat,
	std::vector<pat>::const_iterator aPatEnd
	)
{
#ifdef _DEBUG_FPATTERN
	std::cout << "match_impl(";
	if (anElem != anElemEnd)
		std::cout << std::quoted(*anElem) << " : _";
	else
		std::cout << "[]";
	std::cout << ", ";
	if (aPat != aPatEnd)
		std::cout << *aPat << " : _";
	else
		std::cout << "[]";
	std::cout << ")" <<  std::endl;
#endif

	bool skip1IsSkip = false;
	for (;;) {
		if (anElem != anElemEnd && aPat != aPatEnd
			&& (std::holds_alternative<pat_skip>(*aPat)
				|| (skip1IsSkip && std::holds_alternative<pat_skip1>(*aPat))))
		{
			if (match_impl(anElem, anElemEnd, std::next(aPat), aPatEnd)) {
				return true;
				/* NOTREACHED */
			}
			anElem = std::next(anElem);
			continue;
			/* NOTREACHED */
		}
		if (anElem != anElemEnd && aPat != aPatEnd
			&& !skip1IsSkip && std::holds_alternative<pat_skip1>(*aPat))
		{
			anElem = std::next(anElem);
			skip1IsSkip = true;
			continue;
			/* NOTREACHED */
		}
		if (anElem == anElemEnd && aPat != aPatEnd
			&& (std::holds_alternative<pat_skip>(*aPat)
				|| (skip1IsSkip && std::holds_alternative<pat_skip1>(*aPat))))
		{
			aPat = std::next(aPat);
			skip1IsSkip = false;
			continue;
			/* NOTREACHED */
		}
		if (anElem != anElemEnd && aPat != aPatEnd
			&& std::holds_alternative<pat_star>(*aPat))
		{
			anElem = std::next(anElem);
			aPat = std::next(aPat);
			continue;
			/* NOTREACHED */
		}
		if (anElem != anElemEnd && aPat != aPatEnd
			&& std::holds_alternative<pat_lit>(*aPat))
		{
			const std::string_view& lit = std::get<pat_lit>(*aPat).lit;
			if (lit == *anElem
				&& match_impl(std::next(anElem), anElemEnd, std::next(aPat), aPatEnd))
			{
				return true;
				/* NOTREACHED */
			}
			if (lit != ".") {
				return false;
				/* NOTREACHED */
			}
			aPat = std::next(aPat);
			continue;
			/* NOTREACHED */
		}
		if (anElem != anElemEnd && aPat != aPatEnd
			&& std::holds_alternative<pat_stars>(*aPat))
		{
			if (!match_stars(*anElem, std::get<pat_stars>(*aPat))) {
				return false;
				/* NOTREACHED */
			}
			anElem = std::next(anElem);
			aPat = std::next(aPat);
			continue;
			/* NOTREACHED */
		}
		if (anElem == anElemEnd && aPat == aPatEnd) {
			return true;
			/* NOTREACHED */
		}
		return false;
		/* NOTREACHED */
	}
}

static std::vector<std::string_view> split_path(const std::string_view& aPath)
{
	std::vector<std::string_view> retval;
	size_t i, j, end;

	end = aPath.size();
	i = 0;
	while (i <= end) {
		for (j = i; j < end && !is_path_sep(aPath[j]); ++j);
		retval.push_back(std::string_view(aPath.data() + i, j - i));
		i = j + 1;
	}
	return retval;
}

#ifdef _DEBUG_FPATTERN
static std::ostream& operator<<(std::ostream& os, const std::vector<std::string_view>& xs)
{
	os << "{";
	bool first = true;
	for (const auto& x : xs) {
		if (!first)
			os << ",";
		os << std::quoted(x);
		first = false;
	}
	return os << "}";
}
#endif

static bool match(const filepath& aPath, const std::vector<pat>& Pats)
{
	std::vector<std::string_view> pathElems = split_path(aPath.string());
#ifdef _DEBUG_FPATTERN
	std::cout << "match(" << pathElems << ", " << Pats << ")" << std::endl;
#endif
	return match_impl(pathElems.begin(), pathElems.end(),
		Pats.begin(), Pats.end());
}

bool is_subset_fpattern(const filepath& aPath, const fpattern& aPattern)
{
#ifdef _DEBUG_FPATTERN
	std::cout << "is_subset_fpattern(" << std::quoted(aPath.string()) << ", "
		<< std::quoted(aPattern) << ")" << std::endl;
#endif
	std::vector<pat> pats = optimise(parse(std::string_view(aPattern)));

	if (1 == pats.size()
		&& (std::holds_alternative<pat_skip>(pats[0])
			|| std::holds_alternative<pat_skip1>(pats[0])))
	{
		return (is_relative_pattern(aPattern) ? aPath.is_relative() : true);
		/* NOTREACHED */
	}

	return is_relative_pattern(aPattern) ? aPath.is_relative() && match(aPath, pats)
		                                 : match(aPath, pats);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

