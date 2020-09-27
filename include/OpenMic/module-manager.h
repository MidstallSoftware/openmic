#pragma once

#include <OpenMic/context.h>
#include <OpenMic/module.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_MODULE_MANAGER (openmic_module_manager_get_type())
G_DECLARE_FINAL_TYPE(OpenMicModuleManager, openmic_module_manager, OPENMIC, MODULE_MANAGER, GObject);

struct _OpenMicModuleManagerClass {
	GObjectClass parent_class;
};

struct _OpenMicModuleManager {
	GObject parent_instance;
};

OpenMicModuleManager* openmic_module_manager_new(OpenMicContext* ctx);
void openmic_module_manager_append(OpenMicModuleManager* self, const gchar* path);
void openmic_module_manager_remove(OpenMicModuleManager* self, const gchar* path);
const gchar* openmic_module_manager_find(OpenMicModuleManager* self, const gchar* name);
const OpenMicModule* openmic_module_manager_load(OpenMicModuleManager* self, const gchar* fname);

G_END_DECLS
