#include <OpenMic/context.h>
#include <OpenMic/node.h>
#include <OpenMic/module-manager.h>

enum {
	SIG_0,
	SIG_TREE_BUILD_PRE,
	SIG_TREE_BUILD_POST,
	N_SIGNALS
};

typedef struct _OpenMicContextPrivate {
	GPtrArray* modules;
	GMainLoop* main_loop;
} OpenMicContextPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicContext, openmic_context, G_TYPE_OBJECT);

static guint obj_sigs[N_SIGNALS] = { 0 };

static void openmic_context_dispose(GObject* obj) {}

static void openmic_context_finalize(GObject* obj) {
	OpenMicContext* self = OPENMIC_CONTEXT(obj);
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);

	g_object_unref(self->module_manager);
	g_main_loop_unref(priv->main_loop);
}

static void openmic_context_class_init(OpenMicContextClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = openmic_context_dispose;
	object_class->finalize = openmic_context_finalize;

	obj_sigs[SIG_TREE_BUILD_PRE] = g_signal_newv("tree-build:pre", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);
	obj_sigs[SIG_TREE_BUILD_POST] = g_signal_newv("tree-build:post", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);
}

static void openmic_context_init(OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);

	self->module_manager = openmic_module_manager_new(self);

	priv->main_loop = g_main_loop_new(NULL, FALSE);
}

void openmic_context_build_treev(OpenMicContext* self, va_list ap) {
	if (self->tree) {
		g_node_destroy(self->tree);
	}

	self->tree = g_node_new(NULL); // TODO: OpenMicOutput

	g_signal_emit(self, obj_sigs[SIG_TREE_BUILD_PRE], 0);

	OpenMicNode* node;
	while ((node = va_arg(ap, OpenMicNode*))) {
		OpenMicNodeClass* node_class = OPENMIC_NODE_CLASS(node);
		g_node_insert(self->tree, -1, node_class->gnode);
	}

	g_signal_emit(self, obj_sigs[SIG_TREE_BUILD_POST], 0);
}

void openmic_context_build_tree(OpenMicContext* self, ...) {
	va_list ap;
	va_start(ap, self);
	openmic_context_build_treev(self, ap);
	va_end(ap);
}
