#include <OpenMic/nodes/output.h>

G_DEFINE_TYPE(OpenMicOutput, openmic_output, OPENMIC_TYPE_NODE);

static void openmic_output_class_init(OpenMicOutputClass* klass) {
}

static void openmic_output_init(OpenMicOutput* self) {
	g_object_set(G_OBJECT(self), "element", gst_element_factory_make("autoaudiosink", NULL), NULL);
}
