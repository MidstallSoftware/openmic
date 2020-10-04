#include <OpenMic/nodes/input.h>

typedef struct _OpenMicInputPrivate {
} OpenMicInputPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicInput, openmic_input, OPENMIC_TYPE_NODE);

static void openmic_input_class_init(OpenMicInputClass* klass) {}

static void openmic_input_init(OpenMicInput* self) {
	OpenMicInputPrivate* priv = openmic_input_get_instance_private(self);
}
