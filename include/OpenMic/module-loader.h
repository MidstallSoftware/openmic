#pragma once

#include <OpenMic/context.h>
#include <OpenMic/module.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_MODULE_LOADER (openmic_module_loader_get_type())
G_DECLARE_FINAL_TYPE(OpenMicModuleLoader, openmic_module_loader, OPENMIC, MODULE_LOADER, GTypeModule);

struct _OpenMicModuleLoader {
	GObject parent_instance;
};

OpenMicModuleLoader* openmic_module_loader_new(OpenMicContext* ctx, const gchar* path);
OpenMicModule* openmic_module_loader_create_instance(OpenMicModuleLoader* self);

G_END_DECLS
