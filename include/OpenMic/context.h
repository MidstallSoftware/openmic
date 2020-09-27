#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_CONTEXT (openmic_context_get_type())
G_DECLARE_FINAL_TYPE(OpenMicContext, openmic_context, OPENMIC, CONTEXT, GObject);

struct _OpenMicContextClass {
	GObjectClass parent_class;
};

struct _OpenMicContext {
	GObject parent_instance;
};

typedef struct _OpenMicContextOptions {
	gchar** module_paths;
	gint n_module_paths;
} OpenMicContextOptions;

OpenMicContext* openmic_context_new(OpenMicContextOptions opts);

G_END_DECLS
