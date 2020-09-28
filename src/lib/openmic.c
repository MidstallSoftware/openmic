#include <OpenMic/openmic.h>
#include <gst/gst.h>
#include <openmic-config.h>

static gboolean openmic_inited = FALSE;

GOptionGroup* openmic_get_option_group() {
	GOptionGroup* grp = g_option_group_new("openmic", "OpenMic Library Options", "OpenMic Library Options", NULL, NULL);
	return grp;
}

void openmic_init(int* argc, char** argv[]) {
	GError* error = NULL;
	if (!openmic_init_check(argc, argv, &error)) {
		g_error("Failed to initialize OpenMic: %s", error ? error->message : "unknown error");
		if (error) g_error_free(error);
		exit(1);
	}
}

gboolean openmic_init_check(int* argc, char** argv[], GError** error) {
	static GMutex lock_init;
	gboolean init = FALSE;

	g_mutex_lock(&lock_init);

	if (openmic_inited) {
		g_mutex_unlock(&lock_init);
		return TRUE;
	}

	if (!gst_init_check(argc, argv, error)) {
		g_mutex_unlock(&lock_init);
		return FALSE;
	}

	GOptionContext* ctx = g_option_context_new("- OpenMic Initialization");
	g_option_context_set_ignore_unknown_options(ctx, TRUE);
	g_option_context_set_help_enabled(ctx, FALSE);

	GOptionGroup* grp = openmic_get_option_group();
	g_option_context_add_group(ctx, grp);
	init = g_option_context_parse(ctx, argc, argv, error);
	g_option_context_free(ctx);

	openmic_inited = init;
	g_mutex_unlock(&lock_init);
	return init;
}

void openmic_version(guint* major, guint* minor, guint* patch) {
	*major = OPENMIC_VERSION_MAJOR;
	*minor = OPENMIC_VERSION_MINOR;
	*patch = OPENMIC_VERSION_PATCH;
}

gchar* openmic_version_string() {
	return OPENMIC_VERSION;
}

gboolean openmic_is_initialized() {
	return openmic_inited;
}
