#include <OpenMic/nodes/output.h>

typedef struct _OpenMicOutputPrivate {
} OpenMicOutputPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicOutput, openmic_output, OPENMIC_TYPE_NODE);

static void openmic_output_class_init(OpenMicOutputClass* klass) {
}

static void openmic_output_init(OpenMicOutput* self) {
	OpenMicOutputPrivate* priv = openmic_output_get_instance_private(self);
}
