#pragma once

#include <glib-object.h>
#include <stdarg.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_CONTEXT (openmic_context_get_type())
G_DECLARE_FINAL_TYPE(OpenMicContext, openmic_context, OPENMIC, CONTEXT, GObject);

struct _OpenMicModuleManager;

struct _OpenMicContextClass {
	GObjectClass parent_class;
};

struct _OpenMicContext {
	GObject parent_instance;

	struct _OpenMicModuleManager* module_manager;
	GNode* tree;
};

typedef struct _OpenMicContextOptions {
	gchar** module_paths;
	gint n_module_paths;
} OpenMicContextOptions;

OpenMicContext* openmic_context_new(OpenMicContextOptions opts);
void openmic_context_build_treev(OpenMicContext* self, va_list ap);
void openmic_context_build_tree(OpenMicContext* self, ...);

G_END_DECLS
