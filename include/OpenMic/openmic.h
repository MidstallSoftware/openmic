#pragma once

#include <glib.h>

GOptionGroup* openmic_get_option_group();
void openmic_init(int* argc, char** argv[]);
gboolean openmic_init_check(int* argc, char** argv[], GError** error);
void openmic_version(guint* major, guint* minor, guint* patch);
gchar* openmic_version_string();
gboolean openmic_is_initialized();
