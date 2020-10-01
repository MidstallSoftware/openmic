#pragma once

#include <gst/gst.h>
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

struct _OpenMicNodeClass {
	GObjectClass parent_class;

	GstBin* bin;
	GNode* gnode;

#ifdef OPENMIC_CLI
	GOptionGroup* (*create_cli)(OpenMicNode* node);
#endif
#ifdef OPENMIC_GUI
	GtkWidget* (*create_gui)(OpenMicNode* node);
#endif
#ifdef OPENMIC_TUI
	WINDOW* (*create_tui)(OpenMicNode* node);
#endif
};

G_END_DECLS
