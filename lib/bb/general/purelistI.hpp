
/* -------------------------------------------------------------------------- */

#ifndef __BB_GENERAL_PURELIST_I_HPP__
# define __BB_GENERAL_PURELIST_I_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <memory>
# include <functional>
# include <utility>

/*******************************************************************************
 * %% BeginSection: type/constant declarations
 */

namespace bb::purelist {

template<typename T> struct node;
template<typename T> using list = std::shared_ptr< node<T> >;

template<typename T>
struct node {
	node(const T& aHead, const list<T>& aTail)
		: head(aHead), tail(aTail)
	{}

	T       head;
	list<T> tail;
};

template<typename T>
list<T> cons(const T& x, const list<T>& xs)
{
	return std::make_shared< node<T> >(x, xs);
}

template<typename T>
list<T> nil()
{
	return list<T>();
}

template<typename T>
std::optional<T> headMay(const list<T>& xs)
{
	if (xs) {
		return xs->head;
		/* NOTREACHED */
	}
	return {};
}

template<typename T>
std::optional< list<T> > tailMay(const list<T>& xs)
{
	if (xs) {
		return xs->tail;
		/* NOTREACHED */
	}
	return {};
}

template<typename T>
std::optional< std::pair< T, list<T> > > unconsMay(const list<T>& xs)
{
	if (xs) {
		return std::pair< T, list<T> >(xs->head, xs->tail);
		/* NOTREACHED */
	}
	return {};
}

template<typename T>
std::optional<T>
find(const list<T>& xs, const std::function<bool(const T&)>& pred)
{
	list<T> iter = xs;
	while (iter) {
		if (pred(iter->head)) {
			return iter->head;
			/* NOTREACHED */
		}
		iter = iter->tail;
	}
	return {};
}

}

#endif /* !defined(__BB_GENERAL_PURELIST_I_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

