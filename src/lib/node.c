#include <OpenMic/node.h>

enum {
	PROP_0,
	PROP_CONTEXT,
	PROP_ELEMENT,
	N_PROPS
};

typedef struct _OpenMicNodePrivate {
	GstElement* elem;
	OpenMicContext* ctx;
	GNode* gnode;
} OpenMicNodePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicNode, openmic_node, G_TYPE_OBJECT);

static GParamSpec* obj_props[N_PROPS] = { NULL };

static void openmic_node_dispose(GObject* obj) {
	G_OBJECT_CLASS(openmic_node_parent_class)->dispose(obj);
}

static void openmic_node_finalize(GObject* obj) {
	OpenMicNode* self = OPENMIC_NODE(obj);
	OpenMicNodeClass* klass = OPENMIC_NODE_GET_CLASS(self);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);

	g_node_destroy(priv->gnode);
	gst_object_unref(GST_OBJECT(priv->elem));

	G_OBJECT_CLASS(openmic_node_parent_class)->finalize(obj);
}

static void openmic_node_set_property(GObject* obj, guint propid, const GValue* value, GParamSpec* pspec) {
	OpenMicNode* self = OPENMIC_NODE(obj);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			priv->ctx = g_value_get_object(value);
			break;
		case PROP_ELEMENT:
			if (!priv->elem) {
				priv->elem = g_value_get_object(value);
				break;
			}
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_node_get_property(GObject* obj, guint propid, GValue* value, GParamSpec* pspec) {
	OpenMicNode* self = OPENMIC_NODE(obj);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			g_value_set_object(value, priv->ctx);
			break;
		case PROP_ELEMENT:
			g_value_set_object(value, priv->elem);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_node_class_init(OpenMicNodeClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = openmic_node_dispose;
	object_class->finalize = openmic_node_finalize;
	object_class->set_property = openmic_node_set_property;
	object_class->get_property = openmic_node_get_property;

	obj_props[PROP_CONTEXT] = g_param_spec_object("context", "Context", "OpenMic context instance", OPENMIC_TYPE_CONTEXT, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	obj_props[PROP_ELEMENT] = g_param_spec_object("element", "Element", "GStreamer element", GST_TYPE_ELEMENT, G_PARAM_READWRITE);
	g_object_class_install_properties(object_class, N_PROPS, obj_props);
}

static void openmic_node_init(OpenMicNode* self) {
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	priv->gnode = g_node_new(self);
}

OpenMicNode* openmic_node_new(OpenMicContext* ctx, const gchar* name) {
	GType type = g_type_from_name(name);
	if (type == G_TYPE_NONE) return NULL;
	OpenMicNode* node = OPENMIC_NODE(g_object_new(type, "context", ctx, NULL));
	g_assert(G_TYPE_CHECK_INSTANCE_TYPE(node, OPENMIC_TYPE_NODE));
	return node;
}

OpenMicContext* openmic_node_get_context(OpenMicNode* self) {
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	return priv->ctx;
}

void openmic_node_attach(OpenMicNode* self, gint i, OpenMicNode* other) {
	g_assert(G_TYPE_CHECK_INSTANCE_TYPE(self, OPENMIC_TYPE_NODE));
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	OpenMicNodePrivate* other_priv = openmic_node_get_instance_private(other);

	OpenMicNodeClass* klass = OPENMIC_NODE_GET_CLASS(self);
	OpenMicNodeClass* other_class = OPENMIC_NODE_GET_CLASS(other);

	g_assert(klass);
	g_assert(other_class);

	g_node_insert(priv->gnode, i, openmic_node_get_gnode(self));
	gst_bin_add(GST_BIN(priv->elem), other_priv->elem);
	gst_element_link(priv->elem, other_priv->elem);
}

void openmic_node_remove(OpenMicNode* self, OpenMicNode* other) {
	g_assert(G_TYPE_CHECK_INSTANCE_TYPE(self, OPENMIC_TYPE_NODE));
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	OpenMicNodePrivate* other_priv = openmic_node_get_instance_private(other);

	OpenMicNodeClass* klass = OPENMIC_NODE_GET_CLASS(self);
	OpenMicNodeClass* other_class = OPENMIC_NODE_GET_CLASS(other);

	g_node_unlink(openmic_node_get_gnode(other));
	gst_bin_remove(GST_BIN(priv->elem), other_priv->elem);
	gst_element_unlink(priv->elem, other_priv->elem);
}

GNode* openmic_node_get_gnode(OpenMicNode* self) {
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	return priv->gnode;
}
