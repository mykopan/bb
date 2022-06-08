
#include <bb.h>

int main(int argc, char *argv[])
{
	return bb::args(argc, argv, bb::default_options, [](bb::rules& Rules) {
		bb::want(Rules, { "_build/tarball.tar.gz", "_build/tarball2.tar.gz" });

		bb::file(Rules, "*/*.tar.gz", [](bb::acontext& aCtx, const bb::filepath& aTarget) {
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

#if 0
int main(int argc, char *argv[])
{
	return bpm::args(argc, argv, bpm::default_options, [](bpm::world& aWorld) {
		bpm::name(aWorld, "Vodi.world");
		bpm::version(aWorld, { 2, 14, 0 });
		bpm::build_deps(aWorld, {
			"Bo-2.8.0",
			"Vipm-0.0.1"
			});
		bpm::flags(aWorld,
			bpm::incpath("src/include/Devel") +
			bpm::incpath("src/include")
		);

		/*
		 * Shared libraries
		 */

		bpm::shlib(aWorld, [](bpm::shlib_opt& aShlib) {
			bpm::name(aShlib, "Vodi");
			bpm::version(aShlib, { 2, 5, 0 });
			bpm::rules(aShlib, { bpm::c, bpm::cxx });
			bpm::sources(aShlib, bpm::get_directory_files_recursive("src/lib/libVodi"));
			bpm::flags(aShlib,
				bpm::lshlib("Bo") + bpm::lshlib("m")
			);
		});
	});
}
#endif

