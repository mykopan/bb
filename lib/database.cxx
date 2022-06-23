
/*******************************************************************************
 * %% BeginSection: includes
 */

#include "bb/databaseI.hpp"
#include "bb/errorsI.hpp"

/*******************************************************************************
 * %% BeginSection: function definitions
 */

namespace bb::db {

static void
_t_set_status(
	status_db& aDB,
	id aKeyId,
	const status& aStat
	)
{
	aDB[aKeyId].status = aStat;
}

static void
_t_run(
	database& aDB,
	const build_ops& Ops,
	const stack& aStack,
	id aKeyId
	)
{
	try {
		stack newStack = aStack.push(aKeyId, aDB.status[aKeyId].key);
		try {
			auto buildRes = Ops.build(newStack, aDB.status[aKeyId].key);

			time changed = aDB.now;
			if (std::holds_alternative<loaded>(aDB.status[aKeyId].status)) {
				const result& old =
					std::get<loaded>(aDB.status[aKeyId].status).loaded_result;
				if (old.value == buildRes.first)
					changed = old.changed;
			}

			_t_set_status(aDB.status, aKeyId, ready{ result{
				.value = buildRes.first,
				.built = aDB.now,
				.changed = changed,
				.depends = buildRes.second
			}});
		}
		catch (const bb_exception&) {
			throw;
			/* NOTREACHED */
		}
		catch (const std::exception& err) {
			throw bb_exception(newStack.show(), err);
			/* NOTREACHED */
		}
	}
	catch (...) {
		std::exception_ptr eptr = std::current_exception();
		_t_set_status(aDB.status, aKeyId, error{ eptr });
	}
}

static void
_t_build(database&, const build_ops&, const stack&, const std::vector<id>&);

static bool
_t_skip(
	database& aDB,
	const build_ops& Ops,
	const stack& aStack,
	id aKeyId
	)
{
	stack newStack = aStack.push(aKeyId, aDB.status[aKeyId].key);
	try {
		assert(std::holds_alternative<loaded>(aDB.status[aKeyId].status));
		const result& old =
			std::get<loaded>(aDB.status[aKeyId].status).loaded_result;
		_t_build(aDB, Ops, newStack, old.depends);
	}
	catch (const bb_exception&) {
		throw;
		/* NOTREACHED */
	}
	catch (const std::exception& err) {
		throw bb_exception(newStack.show(), err);
		/* NOTREACHED */
	}

	assert(std::holds_alternative<loaded>(aDB.status[aKeyId].status));
	const result& old =
		std::get<loaded>(aDB.status[aKeyId].status).loaded_result;
	bool skip = true;
	for (auto depId : old.depends) {
		const key_status& depKeyStatus = aDB.status[depId];
		std::visit(overloaded {
			[&skip, &old](const ready& depStat) {
				if (old.built < depStat.ready_result.changed)
					skip = false;
			},
			[&skip](const error&) { skip = false; },
			[](const waiting&) { assert(0 && "not implemented"); },
			[](const missing&) { assert(0); },
			[](const loaded&) { assert(0); }
		}, depKeyStatus.status);
		if (!skip)
			break;
	}
	return skip;
}

static void
_t_check(
	database& aDB,
	const build_ops& Ops,
	const stack& aStack,
	id aKeyId
	)
{
	if (!_t_skip(aDB, Ops, aStack, aKeyId)) {
		_t_run(aDB, Ops, aStack, aKeyId);
		return;
		/* NOTREACHED */
	}

	assert(std::holds_alternative<loaded>(aDB.status[aKeyId].status));
	const result& old =
		std::get<loaded>(aDB.status[aKeyId].status).loaded_result;
	_t_set_status(aDB.status, aKeyId, ready{ old });
}

static void
_t_build1(
	database& aDB,
	const build_ops& Ops,
	const stack& aStack,
	id aKeyId
	)
{
	std::visit(overloaded{
		[&](const missing&) { _t_run(aDB, Ops, aStack, aKeyId); },
		[&](const loaded& stat) {
			bool validStored;
			try {
				validStored = Ops.valid(aDB.status[aKeyId].key,
					stat.loaded_result.value);
			}
			catch (...) {
				validStored = false;
			}
			if (validStored)
				_t_check(aDB, Ops, aStack, aKeyId);
			else
				_t_run(aDB, Ops, aStack, aKeyId);
		},
		[](const ready&) {},
		[](const error&) {},
		[](const waiting&) {}
	}, aDB.status[aKeyId].status);
}

static void
_t_build(
	database& aDB,
	const build_ops& Ops,
	const stack& aStack,
	const std::vector<id>& Keys
	)
{
	auto badKey = aStack.check(Keys);
	if (badKey) {
		throw error_rule_recursion(badKey->second.type_info().name(),
			badKey->second.show());
		/* NOTREACHED */
	}

	for (auto id : Keys)
		_t_build1(aDB, Ops, aStack, id);
}

static id
_t_intern_key(
	intern_db& anInternDB,
	status_db& aStatusDB,
	const dynamic_key& aKey
	)
{
	std::optional<id> mbId = anInternDB.lookup(aKey);
	if (mbId) {
		assert(aStatusDB.lookup(*mbId));
		return *mbId;
		/* NOTREACHED */
	}

	id keyId = anInternDB.add(aKey);
	aStatusDB.insert(keyId, key_status{aKey, missing{}});
	return keyId;
}

std::pair<std::vector<dynamic_value>, depends>
build(
	database& aDB,
	const build_ops& Ops,
	const stack& aStack,
	const std::vector<dynamic_key>& Keys
	)
{
	std::vector<id> keyIds;
	keyIds.reserve(Keys.size());
	for (const dynamic_key& key : Keys)
		keyIds.push_back(_t_intern_key(aDB.intern, aDB.status, key));

	_t_build(aDB, Ops, aStack, keyIds);

	std::vector<dynamic_value> values;
	values.reserve(Keys.size());
	for (auto id : keyIds) {
		std::visit(overloaded{
			[&values](const ready& stat) { values.push_back(stat.ready_result.value); },
			[](const error& err) { std::rethrow_exception(err.eptr); },
			[](const waiting&) { assert(0 && "not implemented"); },
			[](const missing&) { assert(0 && "unreachable"); },
			[](const loaded&) { assert(0 && "unreachable"); }
		}, aDB.status[id].status);
	}
	return std::make_pair(values, keyIds);
}

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

