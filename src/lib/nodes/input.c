#include <OpenMic/nodes/input.h>

G_DEFINE_TYPE(OpenMicInput, openmic_input, OPENMIC_TYPE_NODE);

static void openmic_input_class_init(OpenMicInputClass* klass) {}

static void openmic_input_init(OpenMicInput* self) {
	g_object_set(G_OBJECT(self), "element", gst_element_factory_make("autoaudiosrc", NULL), NULL);
}
