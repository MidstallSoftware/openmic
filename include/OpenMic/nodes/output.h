#pragma once

#include <OpenMic/node.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_OUTPUT (openmic_output_get_type())
G_DECLARE_FINAL_TYPE(OpenMicOutput, openmic_output, OPENMIC, OUTPUT, OpenMicNode);

struct _OpenMicOutput {
	OpenMicNode parent_instance;
};

G_END_DECLS
