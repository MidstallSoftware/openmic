#include <OpenMic/node.h>

typedef struct _OpenMicNodePrivate {} OpenMicNodePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(OpenMicNode, openmic_node, G_TYPE_OBJECT);

static void openmic_node_class_init(OpenMicNodeClass* klass) {}

static void openmic_node_init(OpenMicNode* self) {}

OpenMicNode* openmic_node_new(OpenMicContext* ctx, const gchar* name) {
	GType type = g_type_from_name(name);
	if (type == G_TYPE_NONE) return NULL;
	OpenMicNode* node = g_object_new(type, "context", ctx, NULL);
	g_assert(G_TYPE_CHECK_INSTANCE_TYPE(node, OPENMIC_TYPE_NODE));
	return node;
}
