#define CBS_IMPLEMENTATION
#define CBS_LIBRARY_PATH "../cbs/cbs.h"
#include CBS_LIBRARY_PATH

#define NUM_MAKE_THREADS "6"

#define NASM_VERSION "2.16.03"
#define NASM_PREFIX "nasm-" NASM_VERSION
#define NASM_TAR_GZ NASM_PREFIX ".tar.gz"
#define NASM_URL "https://www.nasm.us/pub/nasm/releasebuilds/" NASM_VERSION \
                 "/" NASM_TAR_GZ

static char *external_dir;

static void init_env_vars(void) {
	putenv(cbs_string_build("PATH=", getenv("PATH"), ":", external_dir, "/ninja"));
	putenv(cbs_string_build("PKG_CONFIG=", external_dir, "/pkgconf/bin/pkgconf"));
	putenv(cbs_string_build("PKG_CONFIG_PATH=",
	                        external_dir, "/glib/lib/pkgconfig:",
	                        external_dir, "/SDL/lib/pkgconfig:",
	                        external_dir, "/SDL_image/lib/pkgconfig"));
}

static void build_ninja(void) {
	cbs_log("Building Ninja...");

	cbs_cd("./ninja");
	if (cbs_files_exist("./ninja")) cbs_run("./ninja", "--quiet");
	else cbs_run("./configure.py", "--bootstrap");
	cbs_cd("..");
}

static void build_meson(void) {
	build_ninja();

	cbs_log("Building Meson...");

	/* Meson doesn't actually need to be built (it has the Python script already
	 * in the repository). This function only exists for dependency transparency
	 */
}

static void build_pkgconf(void) {
	build_meson();

	cbs_log("Building pkgconf...");

	cbs_cd("./pkgconf");
	cbs_run("../meson/meson.py", "setup", "build", "-Dtests=disabled",
	        cbs_string_build("-Dprefix=", external_dir, "/pkgconf"));
	cbs_run("../meson/meson.py", "compile", "-C", "build");
	cbs_run("../meson/meson.py", "install", "--quiet", "-C", "build");
	cbs_cd("..");
}

static void build_glib(void) {
	build_meson();

	cbs_log("Building GLib...");

	cbs_cd("./glib");
	cbs_run("mkdir", "-p", "./build/gio/gdbus-2.0/codegen");
	if (!cbs_files_exist("./build/gio/gdbus-2.0/codegen/packaging"))
		cbs_run("python3", "-m", "pip", "install",
		        "-t", "./build/gio/gdbus-2.0/codegen", "packaging");
	cbs_run("env", cbs_string_build("PYTHONPATH=", external_dir,
	                                "/glib/build/gio/gdbus-2.0/codegen"),
	        "../meson/meson.py", "setup", "build",
	        cbs_string_build("-Dprefix=", external_dir, "/glib"));
	cbs_run("../meson/meson.py", "compile", "-C", "build");
	cbs_run("../meson/meson.py", "install", "--quiet", "-C", "build");
	cbs_cd("..");
}

static void build_sdl(void) {
	cbs_log("Building SDL...");

	cbs_cd("./SDL");
	switch (cbs_run_status("make", "-q", "-t")) {
	case 2:
		cbs_run("./configure", cbs_string_build("--prefix=", external_dir, "/SDL"));
	case 1:
		cbs_run("make", "-j" NUM_MAKE_THREADS);
		cbs_run("make", "install");
	case 0:;
	}
	cbs_cd("..");
}

static void build_sdl_image(void) {
	build_sdl();

	cbs_log("Building SDL_image...");

	cbs_cd("./SDL_image");
	switch (cbs_run_status("make", "-q", "-t", "--dry-run", "--quiet")) {
	case 2:
		cbs_run("./configure",
		        cbs_string_build("--prefix=", external_dir, "/SDL_image"),
		        cbs_string_build("--with-sdl-prefix=", external_dir, "/SDL"));
	case 1: 

		// Hack to get make to not use automake
		cbs_run("touch", "Makefile.in");

		cbs_run("make");
		cbs_run("make", "install");
	case 0:;
	}
	cbs_cd("..");
}

static void build_qemu(void) {
	init_env_vars();
	build_pkgconf();
	build_glib();
	build_sdl();
	build_sdl_image();

	cbs_log("Building QEMU...");
	
	cbs_cd("./qemu");
	cbs_run_status("git", "apply", "../../qemu.patch");
	if (!cbs_files_exist("./python/scripts/tomli"))
		cbs_run("python3", "-m", "pip", "install", "-t", "./python/scripts", "tomli");
	cbs_run("mkdir", "-p", "./build");

	cbs_cd("./build");
	switch (cbs_run_status("make", "-q", "-t", "--dry-run", "--quiet")) {
	case 2:
		cbs_run("../configure", cbs_string_build("--prefix=", external_dir, "/qemu"),
		        "--audio-drv-list=sdl", "--disable-cocoa", "--enable-sdl",
		        "--enable-sdl-image", "--target-list=x86_64-softmmu");
	case 1:
		cbs_run("env", cbs_string_build("PYTHONPATH=", external_dir,
		                                "/glib/build/gio/gdbus-2.0/codegen"),
		        "make", "-j" NUM_MAKE_THREADS);
		cbs_run("make", "install");
	case 0:;
	}
	cbs_cd("..");

	cbs_cd("..");
}

void build_nasm(void) {
	if (!cbs_files_exist(cbs_string_build(external_dir, "/", NASM_PREFIX))) {
		cbs_run("curl", "--output", NASM_TAR_GZ, NASM_URL);
		cbs_run("tar", "-xf", NASM_TAR_GZ);
		cbs_run("rm", "-rf", NASM_TAR_GZ);
	}

	cbs_log("Building NASM...");

	cbs_cd("./" NASM_PREFIX);
	if (cbs_run_status("make", "-q", "-t", "--dry-run", "--quiet") == 2)
		cbs_run("./configure", cbs_string_build("--prefix=", external_dir,
		                                        "/" NASM_PREFIX));
	cbs_run("make");
	cbs_run("make", "install");
	cbs_cd("..");
}

void clean(void) {
	cbs_log("Cleaning submodules...");
	cbs_run("git", "submodule", "deinit", "-f", ".");
	cbs_run("git", "submodule", "update", "--init", "--recursive", "--checkout");

	cbs_log("Cleaning NASM...");
	cbs_run("rm", "-rf", "./" NASM_PREFIX);
}

int main(int argc, char **argv) {
	cbs_rebuild_self(argv);
	cbs_shift_args(&argc, &argv);

	external_dir = getcwd(NULL, 0);

	const char *arg;
	while ((arg = cbs_shift_args(&argc, &argv))) {
		if (cbs_string_eq(arg, "build")) {
			build_qemu();
			build_nasm();
		} else if (cbs_string_eq(arg, "clean")) clean();
		else cbs_error("Invalid subcommand for \"external\"");
	}

	free(external_dir);
	return 0;
}
