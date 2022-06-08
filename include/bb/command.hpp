
/* -------------------------------------------------------------------------- */

#ifndef __BB_COMMAND_HPP__
# define __BB_COMMAND_HPP__

/*******************************************************************************
 * %% BeginSection: includes
 */

# include <bb/types.hpp>
# include <vector>
# include <filesystem>

/*******************************************************************************
 * %% BeginSection: function declarations
 */

namespace bb {

inline void
add_cmd_args(std::vector<std::string>& Args, const std::string& anArg)
{
	// @todo: split anArg by space
	Args.push_back(anArg);
}

inline void
add_cmd_args(std::vector<std::string>& Args, const char* anArg)
{
	add_cmd_args(Args, std::string(anArg));
}

inline void
add_cmd_args(
	std::vector<std::string>& aSrcDst,
	const std::vector<std::string>& aSrc
	)
{
	aSrcDst.insert(aSrcDst.end(), aSrc.begin(), aSrc.end());
}

inline void
add_cmd_args(
	std::vector<std::string>& Args,
	const std::filesystem::path& aPath
	)
{
	Args.push_back(aPath.string());
}

inline void
add_cmd_args(
	std::vector<std::string>& Args,
	const std::vector<std::filesystem::path>& Paths
	)
{
	for (const auto& pth : Paths)
		Args.push_back(pth.string());
}


template<typename ...Args>
void cmd_collect_args(std::vector<std::string>& anArgs)
{
	(void) anArgs;
}

template<typename Head, typename ...Tail>
void
cmd_collect_args(
	std::vector<std::string>& anArgs,
	const Head& aHead,
	const Tail&... aTail
	)
{
	add_cmd_args(anArgs, aHead);
	cmd_collect_args(anArgs, aTail...);
}

extern void run_cmd(acontext&, const std::vector<std::string>&);

template<typename ...Args>
void cmd(acontext& aCtx, const Args&... anArgs)
{
	std::vector<std::string> args;
	cmd_collect_args(args, anArgs...);
	run_cmd(aCtx, args);
}

}

#endif /* !defined(__BB_COMMAND_HPP__) */

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/

