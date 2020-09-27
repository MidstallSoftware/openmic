#include <OpenMic/module.h>

typedef struct _OpenMicModulePrivate {
	void* todo;
} OpenMicModulePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(OpenMicModule, openmic_module, G_TYPE_OBJECT);

static void openmic_module_class_init(OpenMicModuleClass* klass) {}
static void openmic_module_init(OpenMicModule* self) {}

const OpenMicModuleInfo* openmic_module_get_info(OpenMicModule* module) {
	OpenMicModuleClass* klass = OPENMIC_MODULE_GET_CLASS(module);
	if (klass && klass->module_info) {
		return klass->module_info(module);
	}
	return NULL;
}

#ifdef OPENMIC_GUI
GtkWidget* openmic_module_create_gui(OpenMicModule* module) {
	OpenMicModuleClass* klass = OPENMIC_MODULE_GET_CLASS(module);
	if (klass && klass->create_gui) {
		return klass->create_gui(module);
	}
	return NULL;
}
#endif
