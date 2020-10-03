#include <OpenMic/nodes/input.h>
#include <OpenMic/nodes/output.h>
#include <OpenMic/context.h>
#include <OpenMic/node.h>
#include <OpenMic/module-manager.h>
#include <glib/gi18n.h>

enum {
	PROP_0,
	PROP_PIPELINE,
	N_PROPS,

	SIG_0 = 0,
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

static GParamSpec* obj_props[N_PROPS] = { NULL };
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

static void openmic_context_set_property(GObject* obj, guint propid, const GValue* value, GParamSpec* pspec) {
	switch (propid) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_context_get_property(GObject* obj, guint propid, GValue* value, GParamSpec* pspec) {
	OpenMicContext* self = OPENMIC_CONTEXT(obj);
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	switch (propid) {
		case PROP_PIPELINE:
			g_value_set_object(value, priv->pipeline);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_context_class_init(OpenMicContextClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = openmic_context_dispose;
	object_class->finalize = openmic_context_finalize;
	object_class->set_property = openmic_context_set_property;
	object_class->get_property = openmic_context_get_property;

	obj_sigs[SIG_TREE_BUILD_PRE] = g_signal_newv("tree-build-pre", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);
	obj_sigs[SIG_TREE_BUILD_POST] = g_signal_newv("tree-build-post", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);

	obj_props[PROP_PIPELINE] = g_param_spec_object("pipeline", "Pipeline", "GStreamer Pipeline", GST_TYPE_ELEMENT, G_PARAM_READABLE);
	g_object_class_install_properties(object_class, N_PROPS, obj_props);
}

static void openmic_context_init(OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);

	self->module_manager = openmic_module_manager_new(self);

	priv->main_loop = g_main_loop_new(NULL, FALSE);
	priv->types = g_ptr_array_new();
	priv->pipeline = gst_pipeline_new(NULL);

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
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);

	if (self->tree) {
		g_node_destroy(self->tree);
	}

	OpenMicNode* tree = openmic_node_new(self, "OpenMicOutput");
	self->tree = openmic_node_get_gnode(tree);

	g_signal_emit(self, obj_sigs[SIG_TREE_BUILD_PRE], 0);

	OpenMicNode* node;
	while ((node = OPENMIC_NODE(va_arg(ap, OpenMicNode*)))) {
		g_assert(node);
		openmic_node_attach(tree, -1, node);
	}

	GstElement* root_elem = NULL;
	g_object_get(G_OBJECT(self->tree->data), "element", &root_elem, NULL);
	g_assert(root_elem);

	gst_bin_add(GST_BIN(priv->pipeline), root_elem);

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
	g_main_loop_run(priv->main_loop);
}

void openmic_context_stop(OpenMicContext* self) {
	OpenMicContextPrivate* priv = openmic_context_get_instance_private(self);
	g_main_loop_quit(priv->main_loop);
	gst_element_set_state(priv->pipeline, GST_STATE_NULL);
}
