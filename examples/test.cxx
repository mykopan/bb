
#include <bb.hpp>

int main(int argc, char *argv[])
{
	return bb::args(argc, argv, bb::default_options, [](bb::rules& Rules) {
		bb::want(Rules,
			{ "build/example/tarball.tar.gz"
			, "build/example/tarball2.tar.gz"
			});

		bb::file(Rules, "*/*/*.tar.gz", [](bb::acontext& aCtx, const bb::filepath& aTarget) {
			std::vector<bb::filepath> srcs =
				{ bb::filepath("examples") / "file1.txt"
				, bb::filepath("examples") / "file2.txt"
				, bb::filepath("examples") / "file3.txt"
				};
			bb::need(aCtx, srcs);
			bb::info(aCtx, "# tar (for " + aTarget.string() + ")");
			bb::cmd(aCtx, "tar", "-czf", aTarget, srcs);
		});
	});
}

