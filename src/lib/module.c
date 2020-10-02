#include <OpenMic/module.h>

enum {
	PROP_0,
	PROP_CONTEXT,
	N_PROPS
};

typedef struct _OpenMicModulePrivate {
	OpenMicContext* ctx;
} OpenMicModulePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(OpenMicModule, openmic_module, G_TYPE_OBJECT);

static GParamSpec* obj_props[N_PROPS] = { NULL };

static void openmic_module_set_property(GObject* obj, guint propid, const GValue* value, GParamSpec* pspec) {
	OpenMicModule* self = OPENMIC_MODULE(obj);
	OpenMicModulePrivate* priv = openmic_module_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			priv->ctx = g_value_get_object(value);
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
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_module_class_init(OpenMicModuleClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->set_property = openmic_module_set_property;
	object_class->get_property = openmic_module_get_property;

	obj_props[PROP_CONTEXT] = g_param_spec_object("context", "Context", "OpenMic context instance", OPENMIC_TYPE_CONTEXT, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

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

#ifdef OPENMIC_GUI
GtkWidget* openmic_module_create_gui(OpenMicModule* self) {
	OpenMicModuleClass* klass = OPENMIC_MODULE_GET_CLASS(self);
	if (klass && klass->create_gui) {
		return klass->create_gui(self);
	}
	return NULL;
}
#endif
