#include <OpenMic/nodes/input.h>

G_DEFINE_TYPE(OpenMicInput, openmic_input, OPENMIC_TYPE_NODE);

static void openmic_input_class_init(OpenMicInputClass* klass) {
	OpenMicNodeClass* node_class = OPENMIC_NODE_CLASS(klass);
	node_class->elem = gst_element_factory_make("autoaudiosrc", NULL);
}

static void openmic_input_init(OpenMicInput* self) {}
