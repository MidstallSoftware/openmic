#pragma once

#include <OpenMic/context.h>
#include <glib-object.h>
#include <openmic-config.h>

#ifdef OPENMIC_GUI
#include <gtk/gtk.h>
#endif
#ifdef OPENMIC_TUI
#include <ncurses.h>
#endif

G_BEGIN_DECLS

#define OPENMIC_TYPE_NODE (openmic_node_get_type())
G_DECLARE_DERIVABLE_TYPE(OpenMicNode, openmic_node, OPENMIC, NODE, GObject);

typedef struct {
	size_t sample_rate;
	gboolean loop;
} OpenMicNodeStreamInfo;

struct _OpenMicNodeClass {
	GObjectClass parent_class;

#ifdef OPENMIC_CLI
	GOptionGroup* (*create_cli)(OpenMicNode* node);
#endif
#ifdef OPENMIC_GUI
	GtkWidget* (*create_gui)(OpenMicNode* node);
#endif
#ifdef OPENMIC_TUI
	WINDOW* (*create_tui)(OpenMicNode* node);
#endif

	size_t (*read)(OpenMicNode* node, uint16_t** buffer, GError** error);
	void (*stream_info)(OpenMicNode* node, OpenMicNodeStreamInfo* info);

	gpointer padding[12];
};

OpenMicNode* openmic_node_new(OpenMicContext* ctx, const gchar* name);
OpenMicContext* openmic_node_get_context(OpenMicNode* self);
void openmic_node_attach(OpenMicNode* self, gint i, OpenMicNode* other);
void openmic_node_remove(OpenMicNode* self, OpenMicNode* other);
GNode* openmic_node_get_gnode(OpenMicNode* self);
gboolean openmic_node_sync_stream(OpenMicNode* node, GError** error);

G_END_DECLS
