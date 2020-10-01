#include <OpenMic/openmic.h>
#include <glib/gi18n.h>
#include <gst/gst.h>
#include <openmic-config.h>
#include <stdio.h>

static gboolean openmic_inited = FALSE;
static gboolean openmic_show_version = FALSE;

GOptionGroup* openmic_get_option_group() {
	GOptionGroup* grp = g_option_group_new("openmic", g_dgettext(GETTEXT_PACKAGE, "LIB_OPTS"), g_dgettext(GETTEXT_PACKAGE, "LIB_OPTS"), NULL, NULL);
	g_option_group_add_entries(grp, (GOptionEntry[]){
		{ "openmic-version", 0, G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &openmic_show_version, g_dgettext(GETTEXT_PACKAGE, "SHOW_VERSION"), NULL },
		NULL
	});
	return grp;
}

void openmic_init(int* argc, char** argv[]) {
	GError* error = NULL;
	if (!openmic_init_check(argc, argv, &error)) {
		g_error("%s: %s", g_dgettext(GETTEXT_PACKAGE, "INIT_FAILURE"), error ? error->message : g_dgettext(GETTEXT_PACKAGE, "ERROR_UNKNOWN"));
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

	bindtextdomain(GETTEXT_PACKAGE, OPENMIC_DATADIR"/local");
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

	GOptionContext* ctx = g_option_context_new(g_dgettext(GETTEXT_PACKAGE, "ARGS_LABEL"));
	g_option_context_set_ignore_unknown_options(ctx, TRUE);
	g_option_context_set_help_enabled(ctx, FALSE);

	GOptionGroup* grp = openmic_get_option_group();
	g_option_context_add_group(ctx, grp);
	init = g_option_context_parse(ctx, argc, argv, error);
	g_option_context_free(ctx);

	if (openmic_show_version) {
		printf("OpenMic Library Version: %s\n", openmic_version_string());
		exit(0);
	}

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
