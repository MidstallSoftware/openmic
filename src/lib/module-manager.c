#include <OpenMic/context.h>
#include <OpenMic/module-loader.h>
#include <OpenMic/module-manager.h>

enum {
	PROP_0,
	PROP_CONTEXT,
	N_PROPS,

	SIG_0 = 0,
	SIG_LOAD,
	N_SIGNALS
};

typedef struct _OpenMicModuleManagerPrivate {
	OpenMicContext* ctx;
	GPtrArray* paths;
	GPtrArray* loaders;
} OpenMicModuleManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicModuleManager, openmic_module_manager, G_TYPE_OBJECT);

static GParamSpec* obj_props[N_PROPS] = { NULL };
static guint obj_sigs[N_SIGNALS] = { 0 };

static void openmic_module_manager_dispose(GObject* obj) {
	G_OBJECT_CLASS(openmic_module_manager_parent_class)->dispose(obj);
}

static void openmic_module_manager_finalize(GObject* obj) {
	OpenMicModuleManager* self = OPENMIC_MODULE_MANAGER(obj);
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);

	g_ptr_array_unref(priv->paths);
	g_ptr_array_unref(priv->loaders);

	G_OBJECT_CLASS(openmic_module_manager_parent_class)->finalize(obj);
}

static void openmic_module_manager_set_property(GObject* obj, guint propid, const GValue* value, GParamSpec* pspec) {
	OpenMicModuleManager* self = OPENMIC_MODULE_MANAGER(obj);
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			priv->ctx = g_value_get_object(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_module_manager_get_property(GObject* obj, guint propid, GValue* value, GParamSpec* pspec) {
	OpenMicModuleManager* self = OPENMIC_MODULE_MANAGER(obj);
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			g_value_set_object(value, priv->ctx);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_module_manager_class_init(OpenMicModuleManagerClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = openmic_module_manager_dispose;
	object_class->finalize = openmic_module_manager_finalize;
	object_class->set_property = openmic_module_manager_set_property;
	object_class->get_property = openmic_module_manager_get_property;

	obj_sigs[SIG_LOAD] = g_signal_new("load", G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 1, OPENMIC_TYPE_MODULE);
	obj_props[PROP_CONTEXT] = g_param_spec_object("context", "Context", "OpenMic context instance", OPENMIC_TYPE_CONTEXT, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_properties(object_class, N_PROPS, obj_props);
}

static void openmic_module_manager_init(OpenMicModuleManager* self) {
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);

	priv->paths = g_ptr_array_new_with_free_func(g_free);
	priv->loaders = g_ptr_array_new_with_free_func(g_object_unref);
}

OpenMicModuleManager* openmic_module_manager_new(OpenMicContext* ctx) {
	return g_object_new(OPENMIC_TYPE_MODULE_MANAGER, "context", ctx, NULL);
}

void openmic_module_manager_append(OpenMicModuleManager* self, const gchar* path) {
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);
	g_ptr_array_add(priv->paths, g_strdup(path));
}

void openmic_module_manager_remove(OpenMicModuleManager* self, const gchar* path) {
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);
	guint index;
	if (g_ptr_array_find_with_equal_func(priv->paths, path, g_str_equal, &index)) {
		g_ptr_array_remove_index(priv->paths, index);
	}
}

const gchar* openmic_module_manager_find(OpenMicModuleManager* self, const gchar* name) {
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);
	for (guint i = 0; i < priv->paths->len; i++) {
		gchar* str = g_strdup(g_ptr_array_index(priv->paths, i));
		gchar* path = g_strdup_printf("%s/%s."OPENMIC_MODULE_EXT, str, name);
		g_free(str);
		if (!path) continue;
		if (!g_file_test(path, G_FILE_TEST_IS_REGULAR | G_FILE_TEST_IS_EXECUTABLE | G_FILE_TEST_IS_SYMLINK | G_FILE_TEST_EXISTS)) {
			g_free(path);
			continue;
		}
		return path;
	}
	return NULL;
}

const OpenMicModule* openmic_module_manager_load(OpenMicModuleManager* self, const gchar* fname) {
	OpenMicModuleManagerPrivate* priv = openmic_module_manager_get_instance_private(self);
	const gchar* modpath = openmic_module_manager_find(self, fname);
	if (!modpath) return NULL;
	OpenMicModuleLoader* loader = openmic_module_loader_new(priv->ctx, modpath);
	g_ptr_array_add(priv->loaders, loader);
	OpenMicModule* mod = openmic_module_loader_create_instance(loader);
	g_signal_emit(self, obj_sigs[SIG_LOAD], 0, mod);
	return mod;
}
