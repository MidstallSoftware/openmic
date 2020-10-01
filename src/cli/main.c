#include <OpenMic/openmic.h>
#include <gst/gst.h>
#include <glib.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	openmic_init(&argc, &argv);

	GOptionContext* ctx = g_option_context_new("- OpenMic CLI");
	GOptionGroup* grp = openmic_get_option_group();
	g_option_context_add_group(ctx, grp);
	g_option_context_add_group(ctx, gst_init_get_option_group());

	GError* error = NULL;
	if (!g_option_context_parse(ctx, &argc, &argv, &error)) {
		g_error("Failed to parse arguments: %s", error->message);
		g_error_free(error);
		return EXIT_FAILURE;
	}

	g_option_context_free(ctx);
	return EXIT_SUCCESS;
}
