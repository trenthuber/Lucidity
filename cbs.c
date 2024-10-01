#define CBS_IMPLEMENTATION
#define CBS_LIBRARY_PATH "./external/cbs.d/cbs.h"
#include CBS_LIBRARY_PATH

#define CC "cc"

static const char *program;

void build_lucidity(void) {
	cbs_log("Todo! Will build lucidity eventually");
}

void run_lucidity(void) {
	if (!cbs_files_exist("./external/qemu/bin/qemu-system-x86_64"))
		cbs_error("QEMU wasn't found. Make sure it's built by running \"", program, " external build\"");
	cbs_run("./external/qemu/bin/qemu-system-x86_64");
}

void clean_lucidity(void) {
	cbs_log("Todo! Will clean Lucidity once there's something to clean");
}

void clean_cbs(void) {
	cbs_run("rm", "-f", "./external/cbs.d/cbs");

	cbs_run("rm", "-f", program);
}

int main(int argc, char **argv) {
	cbs_rebuild_self(argv);
	program = cbs_shift_args(&argc, &argv);

	const char *arg;
	while ((arg = cbs_shift_args(&argc, &argv)))
		if (cbs_string_eq(arg, "build")) {
			cbs_subbuild("external", "build");
			build_lucidity();
		} else if (cbs_string_eq(arg, "clean")) {
			cbs_subbuild("external", "clean");
			clean_lucidity();
			clean_cbs();
		} else if (cbs_string_eq(arg, "lucidity"))
			if ((arg = cbs_shift_args(&argc, &argv)) == NULL
			    || cbs_string_eq(arg, "build")) build_lucidity();
			else if (cbs_string_eq(arg, "run")) run_lucidity();
			else if (cbs_string_eq(arg, "clean")) clean_lucidity();
			else cbs_error(cbs_string_build("Invalid subcommand for \"", program,
			                                "lucidity\""));
		else if (cbs_string_eq(arg, "external"))
			cbs_subbuild("external",
			             (arg = cbs_shift_args(&argc, &argv)) ? arg : "build");
		else if (cbs_string_eq(arg, "cbs"))
			if ((arg = cbs_shift_args(&argc, &argv)) == NULL)
				cbs_error(cbs_string_build("Must provide a subcommand for \"", program,
				                           "cbs\""));
			else if (cbs_string_eq(arg, "clean")) clean_cbs();
			else if (cbs_string_eq(arg, "reinit")) {
				cbs_cd("./external/cbs.d");
				cbs_run("git", "stash");
				cbs_run("git", "pull", "origin", "main");
				cbs_cd("../..");
			} else cbs_error(cbs_string_build("Invalid subcommand for \"", program,
			                                  "cbs\""));
		else cbs_error(cbs_string_build("Unknown subcommand: ", arg));

	return 0;
}
