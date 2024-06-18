#define CBS_IMPLEMENTATION
#define CBS_LIBRARY_PATH "./cbs/cbs.h"
#include CBS_LIBRARY_PATH

#define CC "cc"

void update_cbs(void) {
	cbs_cd("./cbs");
	cbs_run("git", "stash");
	cbs_cd("..");
	cbs_run("git", "submodule", "update", "--remote");
}

void build_external(void) {
	cbs_run("git", "submodule", "update", "--init", "--recursive");
	cbs_subbuild("./external", "build");
}

void build_lucidity(void) {
	cbs_log("Todo! Will build lucidity eventually");
}

void build_all(void) {
	update_cbs();
	build_external();
	build_lucidity();
}

void run(void) {
	if (!cbs_files_exist("./external/qemu/bin/qemu-system-x86_64"))
		cbs_error("QEMU wasn't found. Build with \"./lucidity build deps\"");
	cbs_run("./external/qemu/bin/qemu-system-x86_64");
}

void clean_external(void) {
	cbs_subbuild("./external", "clean");
}

void clean_lucidity(void) {
	cbs_log("Todo! Will clean Lucidity once there's something to clean");
}

void clean_cbs(void) {
	cbs_run("rm", "-f", "./external/cbs");

	cbs_run("rm", "-f", "./lucidity");
}

void clean_all(void) {
	clean_external();
	clean_lucidity();
	clean_cbs();
}

int main(int argc, char **argv) {
	cbs_rebuild_self(argv);
	cbs_shift_args(&argc, &argv);

	const char *arg;
	while ((arg = cbs_shift_args(&argc, &argv))) {
	top:
		if (cbs_string_eq(arg, "build")) {
			if ((arg = cbs_shift_args(&argc, &argv)) == NULL) {
				build_lucidity();
				break;
			} else if (cbs_string_eq(arg, "all")) build_all();
			else if (cbs_string_eq(arg, "deps")) build_external();
			else if (cbs_string_eq(arg, "os")) build_lucidity();
			else {
				build_lucidity();
				goto top;
			}
		}
		else if (cbs_string_eq(arg, "run")) run();
		else if (cbs_string_eq(arg, "clean")) {
			if ((arg = cbs_shift_args(&argc, &argv)) == NULL) {
				clean_lucidity();
				break;
			} else if (cbs_string_eq(arg, "all")) clean_all();
			else if (cbs_string_eq(arg, "deps")) clean_external();
			else if (cbs_string_eq(arg, "os")) clean_lucidity();
			else if (cbs_string_eq(arg, "cbs")) clean_cbs();
			else {
				clean_lucidity();
				goto top;
			}
		}
		else if (cbs_string_eq(arg, "cbs")) update_cbs();
		else cbs_error(cbs_string_build("Unknown subcommand: ", arg));
	}

	return 0;
}
