#include <OpenMic/context.h>
#include <OpenMic/module-manager.h>
#include <OpenMic/openmic.h>
#include <openmic-config.h>
#include <gst/gst.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <locale.h>
#include <stdlib.h>


int main(int argc, char** argv) {
	openmic_init(&argc, &argv);

	setlocale(LC_ALL, "");
	bindtextdomain(GETTEXT_PACKAGE, OPENMIC_DATADIR"/locale");
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	gchar** modules = NULL;
	gboolean show_nodes = FALSE;

	{
		GOptionContext* ctx = g_option_context_new(_("ARGS_LABEL"));
		g_option_context_add_group(ctx, openmic_get_option_group());
		g_option_context_add_group(ctx, gst_init_get_option_group());

		GOptionGroup* grp = g_option_group_new(NULL, NULL, NULL, NULL, NULL);
		g_option_group_add_entries(grp, (GOptionEntry[]){
			{ "modules", 'm', G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING_ARRAY, &modules, _("ARGS_MODULES"), "LIST" },
			{ "list-nodes", 0, G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &show_nodes, _("ARGS_LIST_NODES"), NULL },
			NULL
		});
		g_option_context_set_main_group(ctx, grp);

		GError* error = NULL;
		if (!g_option_context_parse(ctx, &argc, &argv, &error)) {
			g_error("Failed to parse arguments: %s", error->message);
			g_error_free(error);
			return EXIT_FAILURE;
		}
		g_option_context_free(ctx);
	}

	OpenMicContextOptions opts = {};
	opts.module_paths = malloc(sizeof(char) * 2);
	opts.module_paths[0] = g_build_path(OPENMIC_PATH_SEP, g_get_home_dir(), ".openmic", "modules");
	opts.module_paths[1] = NULL;
	OpenMicContext* ctx = openmic_context_new(opts);
	g_strfreev(opts.module_paths);

	if (modules) {
		gchar* str;
		guint i = 0;
		while ((str = modules[i++])) {
			openmic_module_manager_load(ctx->module_manager, str);
		}
		g_strfreev(modules);
	}

	if (show_nodes) {
		guint n_types = 0;
		GType* types = g_type_children(OPENMIC_TYPE_NODE, &n_types);
		// TODO: use context's built in registry

		for (guint i = 0; i < n_types; i++) {
			printf("%s\n", g_type_name(types[i]));
		}

		g_free(types);
		g_object_unref(ctx);
		return EXIT_SUCCESS;
	}

	openmic_context_play(ctx);
	g_object_unref(ctx);
	return EXIT_SUCCESS;
}
