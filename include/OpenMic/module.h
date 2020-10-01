#pragma once

#include <openmic-config.h>
#include <glib-object.h>
#include <OpenMic/context.h>
#include <OpenMic/node.h>

#ifdef OPENMIC_GUI
#include <gtk/gtk.h>
#endif
#ifdef OPENMIC_TUI
#include <ncurses.h>
#endif

G_BEGIN_DECLS

#define OPENMIC_TYPE_MODULE (openmic_module_get_type())
G_DECLARE_DERIVABLE_TYPE(OpenMicModule, openmic_module, OPENMIC, MODULE, GObject);

typedef struct _OpenMicModuleInfo {
	const gchar* name;
	const gchar* author;
	const gchar* verion;
	const gchar* homepage;
	const gchar* license;
	const gchar* description;
} OpenMicModuleInfo;

struct _OpenMicModuleClass {
	GObjectClass parent_class;

	const OpenMicModuleInfo* (*module_info)(OpenMicModule* module);
	OpenMicNode* (*apply)(OpenMicModule* module, OpenMicNode* node);

#ifdef OPENMIC_CLI
	GOptionGroup* (*create_cli)(OpenMicModule* module);
#endif
#ifdef OPENMIC_GUI
	GtkWidget* (*create_gui)(OpenMicModule* module);
#endif
#ifdef OPENMIC_TUI
	WINDOW* (*create_tui)(OpenMicModule* module);
#endif
};

#define OPENMIC_MODULE_DECLARE_WITH_CODE(ObjectName, object_name, CODE) G_MODULE_EXPORT GType object_name##_get_type(); \
	G_MODULE_EXPORT GType openmic_module_register_type(GTypeModule* type_module); \
	G_DEFINE_DYNAMIC_TYPE_EXTENDED(ObjectName, object_name, OPENMIC_TYPE_MODULE, 0, CODE); \
	static void object_name##_class_finalize(ObjectName##Class* klass) {} \
	G_MODULE_EXPORT GType openmic_module_register_type(GTypeModule* type_module) { \
		object_name##_register_type(type_module); \
		register_extra_types(type_module); \
		return object_name##_get_type(); \
	}

#define OPENMIC_MODULE_DECLARE(ObjectName, object_name) OPENMIC_MODULE_DECLARE_WITH_CODE(ObjectName, object_name, {})

const OpenMicModuleInfo* openmic_module_get_info(OpenMicModule* module);

#ifdef OPENMIC_GUI
GtkWidget* openmic_module_create_gui(OpenMicModule* module);
#endif

G_END_DECLS
