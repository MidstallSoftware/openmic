#include <OpenMic/module-loader.h>

enum {
	PROP_CONTEXT = 1,
	PROP_PATH,
	N_PROPS
};

typedef struct _OpenMicModuleLoaderPrivate {
	GModule* lib;
	gchar* path;
	GType type;
	OpenMicContext* ctx;
} OpenMicModuleLoaderPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicModuleLoader, openmic_module_loader, G_TYPE_TYPE_MODULE);

static GParamSpec* obj_props[N_PROPS] = { NULL };

static void openmic_module_loader_finalize(GObject* obj) {
	G_OBJECT_CLASS(openmic_module_loader_parent_class)->finalize(obj);
}

static void openmic_module_loader_dispose(GObject* obj) {
	G_OBJECT_CLASS(openmic_module_loader_parent_class)->dispose(obj);
}

static void openmic_module_loader_set_property(GObject* obj, guint propid, const GValue* value, GParamSpec* pspec) {
	OpenMicModuleLoader* self = OPENMIC_MODULE_LOADER(obj);
	OpenMicModuleLoaderPrivate* priv = openmic_module_loader_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			priv->ctx = g_value_get_object(value);
			break;
		case PROP_PATH:
			g_free(priv->path);
			priv->path = g_value_dup_string(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_module_loader_get_property(GObject* obj, guint propid, GValue* value, GParamSpec* pspec) {
	OpenMicModuleLoader* self = OPENMIC_MODULE_LOADER(obj);
	OpenMicModuleLoaderPrivate* priv = openmic_module_loader_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			g_value_set_object(value, priv->ctx);
			break;
		case PROP_PATH:
			g_value_set_string(value, priv->path);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static gboolean openmic_module_loader_load(GTypeModule* gmodule) {
	OpenMicModuleLoader* self = OPENMIC_MODULE_LOADER(gmodule);
	OpenMicModuleLoaderPrivate* priv = openmic_module_loader_get_instance_private(self);
	GType (*register_type)(GTypeModule*) = NULL;
	void (*register_extra_types)(GTypeModule*) = NULL;
	if (priv->lib && priv->type) return TRUE;
	g_assert(priv->path);
	if (!priv->lib && !(priv->lib = g_module_open(priv->path, 0))) {
		g_error("Failed to open library \"%s\": %s", priv->path, g_module_error());
		return FALSE;
	}
	if (g_module_symbol(priv->lib, "openmic_module_register_type", (gpointer*)(void*)&register_type) && register_type) {
		GType type = register_type(gmodule);
		if (!type) {
			g_error("Failed to register type \"%s\"", priv->path);
			return FALSE;
		}
		if (g_module_symbol(priv->lib, "register_extra_types", (gpointer*)(void*)&register_extra_types) && register_extra_types) {
			register_extra_types(gmodule);
		}
		priv->type = type;
		return TRUE;
	}
	g_error("Failed to load module \"%s\"", priv->path);
	return FALSE;
}

static void openmic_module_loader_unload(GTypeModule* gmodule) {
	OpenMicModuleLoader* self = OPENMIC_MODULE_LOADER(gmodule);
	OpenMicModuleLoaderPrivate* priv = openmic_module_loader_get_instance_private(self);

	g_module_close(priv->lib);
	priv->lib = NULL;
	priv->type = 0;
}

static void openmic_module_loader_class_init(OpenMicModuleLoaderClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);
	GTypeModuleClass* gmodule_class = G_TYPE_MODULE_CLASS(klass);

	object_class->finalize = openmic_module_loader_finalize;
	object_class->dispose = openmic_module_loader_dispose;
	object_class->set_property = openmic_module_loader_set_property;
	object_class->get_property = openmic_module_loader_get_property;

	gmodule_class->load = openmic_module_loader_load;
	gmodule_class->unload = openmic_module_loader_unload;

	obj_props[PROP_CONTEXT] = g_param_spec_object("context", "Context", "OpenMic context instance", OPENMIC_TYPE_CONTEXT, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	obj_props[PROP_PATH] = g_param_spec_string("path", "Path", "Module load path", NULL, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, N_PROPS, obj_props);
}

static void openmic_module_loader_init(OpenMicModuleLoader* self) {}

OpenMicModuleLoader* openmic_module_loader_new(OpenMicContext* ctx, const gchar* path) {
	return g_object_new(OPENMIC_TYPE_MODULE_LOADER, "context", ctx, "path", path, NULL);
}

OpenMicModule* openmic_module_loader_create_instance(OpenMicModuleLoader* self) {
	OpenMicModuleLoaderPrivate* priv = openmic_module_loader_get_instance_private(self);
	return g_object_new(priv->type, "context", priv->ctx, NULL);
}
