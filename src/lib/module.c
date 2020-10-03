#include <OpenMic/module.h>
#include <OpenMic/module-loader.h>

enum {
	PROP_0,
	PROP_CONTEXT,
	PROP_MODULE_LOADER,
	N_PROPS,

	SIG_0 = 0,
	SIG_UNLOAD,
	N_SIGNALS
};

typedef struct _OpenMicModulePrivate {
	OpenMicContext* ctx;
	OpenMicModuleLoader* loader;
} OpenMicModulePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(OpenMicModule, openmic_module, G_TYPE_OBJECT);

static GParamSpec* obj_props[N_PROPS] = { NULL };
static guint obj_sigs[N_SIGNALS] = { 0 };

static void openmic_module_dispose(GObject* obj) {
	G_OBJECT_CLASS(openmic_module_parent_class)->dispose(obj);
}

static void openmic_module_finalize(GObject* obj) {
	OpenMicModule* self = OPENMIC_MODULE(obj);
	g_signal_emit(self, obj_sigs[SIG_UNLOAD], 0, self);
	G_OBJECT_CLASS(openmic_module_parent_class)->finalize(obj);
}

static void openmic_module_set_property(GObject* obj, guint propid, const GValue* value, GParamSpec* pspec) {
	OpenMicModule* self = OPENMIC_MODULE(obj);
	OpenMicModulePrivate* priv = openmic_module_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			priv->ctx = g_value_get_object(value);
			break;
		case PROP_MODULE_LOADER:
			priv->loader = g_value_get_object(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_module_get_property(GObject* obj, guint propid, GValue* value, GParamSpec* pspec) {
	OpenMicModule* self = OPENMIC_MODULE(obj);
	OpenMicModulePrivate* priv = openmic_module_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			g_value_set_object(value, priv->ctx);
			break;
		case PROP_MODULE_LOADER:
			g_value_set_object(value, priv->loader);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_module_class_init(OpenMicModuleClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = openmic_module_dispose;
	object_class->finalize = openmic_module_finalize;
	object_class->set_property = openmic_module_set_property;
	object_class->get_property = openmic_module_get_property;

	obj_props[PROP_CONTEXT] = g_param_spec_object("context", "Context", "OpenMic context instance", OPENMIC_TYPE_CONTEXT, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	obj_props[PROP_MODULE_LOADER] = g_param_spec_object("module-loader", "Module Loader", "OpenMic Module Loader instance", OPENMIC_TYPE_MODULE_LOADER, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	obj_sigs[SIG_UNLOAD] = g_signal_new("unload", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);

	g_object_class_install_properties(object_class, N_PROPS, obj_props);
}

static void openmic_module_init(OpenMicModule* self) {}

OpenMicContext* openmic_module_get_context(OpenMicModule* self) {
	OpenMicModulePrivate* priv = openmic_module_get_instance_private(self);
	return priv->ctx;
}

const OpenMicModuleInfo* openmic_module_get_info(OpenMicModule* self) {
	OpenMicModuleClass* klass = OPENMIC_MODULE_GET_CLASS(self);
	if (klass && klass->module_info) {
		return klass->module_info(self);
	}
	return NULL;
}

#ifdef OPENMIC_CLI
GOptionGroup* openmic_module_create_cli(OpenMicModule* self) {
	OpenMicModuleClass* klass = OPENMIC_MODULE_GET_CLASS(self);
	if (klass && klass->create_cli) {
		return klass->create_cli(self);
	}
	return NULL;
}
#endif
#ifdef OPENMIC_GUI
GtkWidget* openmic_module_create_gui(OpenMicModule* self) {
	OpenMicModuleClass* klass = OPENMIC_MODULE_GET_CLASS(self);
	if (klass && klass->create_gui) {
		return klass->create_gui(self);
	}
	return NULL;
}
#endif
#ifdef OPENMIC_TUI
WINDOW* openmic_module_create_tui(OpenMicModule* self) {
	OpenMicModuleClass* klass = OPENMIC_MODULE_GET_CLASS(self);
	if (klass && klass->create_tui) {
		return klass->create_tui(self);
	}
	return NULL;
}
#endif
