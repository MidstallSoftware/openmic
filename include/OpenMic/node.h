#pragma once

#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_NODE (openmic_node_get_type())
G_DECLARE_DERIVABLE_TYPE(OpenMicNode, openmic_node, OPENMIC, NODE, GObject);

struct _OpenMicNodeClass {
	GObjectClass parent_class;

	GstBin* bin;
	GNode* gnode;
};

G_END_DECLS
