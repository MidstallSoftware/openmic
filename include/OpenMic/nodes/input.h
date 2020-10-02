#pragma once

#include <OpenMic/node.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_INPUT (openmic_input_get_type())
G_DECLARE_FINAL_TYPE(OpenMicInput, openmic_input, OPENMIC, INPUT, OpenMicNode);

struct _OpenMicInput {
	OpenMicNode parent_instance;
};

G_END_DECLS
