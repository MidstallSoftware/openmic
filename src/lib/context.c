#include <OpenMic/nodes/input.h>
#include <OpenMic/nodes/output.h>
#include <OpenMic/context.h>
#include <OpenMic/node.h>
#include <OpenMic/module-manager.h>
#include <glib/gi18n.h>

enum {
	SIG_0,
	SIG_TREE_BUILD_PRE,
	SIG_TREE_BUILD_POST,
	N_SIGNALS
};

typedef struct _OpenMicContextPrivate {
	GPtrArray* modules;
	GPtrArray* types;
	GMainLoop* main_loop;
	GstElement* pipeline;
	OpenMicContextOptions opts;
	guint bus_watch_id;
} OpenMicContextPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicContext, openmic_context, G_TYPE_OBJECT);

static guint obj_sigs[N_SIGNALS] = { 0 };

static gboolean openmic_context_bus_call(GstBus* bus, GstMessage* msg, gpointer data) {
	OpenMicContext* self = (OpenMicContext*)data;
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS:
			g_main_loop_quit(priv->main_loop);
			break;
		case GST_MESSAGE_ERROR:
			{
				GError* error = NULL;
				gchar* debug = NULL;
				gst_message_parse_error(msg, &error, &debug);
				g_free(debug);
				g_error("OpenMicContext: %s", error->message);
				g_error_free(error);
			}
			g_main_loop_quit(priv->main_loop);
			break;
		default:
			break;
	}
	return TRUE;
}

static void openmic_context_signal_module_load(OpenMicModule* module, OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	g_ptr_array_add(priv->modules, module);
}

static void openmic_context_signal_module_unload(OpenMicModule* module, OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	g_ptr_array_remove(priv->modules, module);
}

static void openmic_context_dispose(GObject* obj) {
	G_OBJECT_CLASS(openmic_context_parent_class)->dispose(obj);
}

static void openmic_context_finalize(GObject* obj) {
	OpenMicContext* self = OPENMIC_CONTEXT(obj);
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);

	g_object_unref(self->module_manager);
	g_ptr_array_unref(priv->types);
	g_main_loop_unref(priv->main_loop);
	gst_object_unref(GST_OBJECT(priv->pipeline));
	g_source_remove(priv->bus_watch_id);

	G_OBJECT_CLASS(openmic_context_parent_class)->finalize(obj);
}

static void openmic_context_class_init(OpenMicContextClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = openmic_context_dispose;
	object_class->finalize = openmic_context_finalize;

	obj_sigs[SIG_TREE_BUILD_PRE] = g_signal_newv("tree-build-pre", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);
	obj_sigs[SIG_TREE_BUILD_POST] = g_signal_newv("tree-build-post", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);
}

static void openmic_context_init(OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);

	self->module_manager = openmic_module_manager_new(self);

	priv->main_loop = g_main_loop_new(NULL, FALSE);
	priv->types = g_ptr_array_new();
	priv->pipeline = gst_pipeline_new("OpenMic Pipeline");

	GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(priv->pipeline));
	priv->bus_watch_id = gst_bus_add_watch(bus, openmic_context_bus_call, self);
	gst_object_unref(bus);

	g_signal_connect(self->module_manager, "load", (GCallback)openmic_context_signal_module_load, self);
	g_signal_connect(self->module_manager, "unload", (GCallback)openmic_context_signal_module_unload, self);

	openmic_context_register_node(self, OPENMIC_TYPE_INPUT);
	openmic_context_register_node(self, OPENMIC_TYPE_OUTPUT);
}

OpenMicContext* openmic_context_new(OpenMicContextOptions opts) {
	OpenMicContext* self = g_object_new(OPENMIC_TYPE_CONTEXT, NULL);
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);

	priv->opts = opts;

	const gchar* str = NULL;
	guint i = 0;
	while ((str = opts.module_paths[i++])) {
		openmic_module_manager_append(self->module_manager, str);
	}
	return self;
}

void openmic_context_build_treev(OpenMicContext* self, va_list ap) {
	if (self->tree) {
		g_node_destroy(self->tree);
	}

	self->tree = g_node_new(OPENMIC_NODE(g_object_new(OPENMIC_TYPE_OUTPUT, NULL)));

	g_signal_emit(self, obj_sigs[SIG_TREE_BUILD_PRE], 0);

	OpenMicNode* node;
	while ((node = OPENMIC_NODE(va_arg(ap, OpenMicNode*)))) {
		g_assert(node);
		openmic_node_attach(self->tree->data, -1, node);
	}

	g_signal_emit(self, obj_sigs[SIG_TREE_BUILD_POST], 0);
}

void openmic_context_build_tree(OpenMicContext* self, ...) {
	va_list ap;
	va_start(ap, self);
	openmic_context_build_treev(self, ap);
	va_end(ap);
}

void openmic_context_register_node(OpenMicContext* self, GType type) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	g_ptr_array_add(priv->types, (gpointer)g_type_name(type));
}

void openmic_context_unregister_node(OpenMicContext* self, GType type) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	g_ptr_array_remove(priv->types, (gpointer)g_type_name(type));
}

void openmic_context_play(OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	gst_element_set_state(priv->pipeline, GST_STATE_PLAYING);
	if (!g_main_loop_is_running(priv->main_loop)) g_main_loop_run(priv->main_loop);
}

void openmic_context_stop(OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	if (g_main_loop_is_running(priv->main_loop)) g_main_loop_quit(priv->main_loop);
	gst_element_set_state(priv->pipeline, GST_STATE_NULL);
}
