#include <OpenMic/nodes/output.h>

G_DEFINE_TYPE(OpenMicOutput, openmic_output, OPENMIC_TYPE_NODE);

static void openmic_output_class_init(OpenMicOutputClass* klass) {
	OpenMicNodeClass* node_class = OPENMIC_NODE_CLASS(klass);
	node_class->elem = gst_element_factory_make("autoaudiosink", NULL);
}

static void openmic_output_init(OpenMicOutput* self) {}
