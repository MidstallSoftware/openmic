#include <OpenMic/nodes/input.h>

typedef struct _OpenMicInputPrivate {
	GstElement* src;
} OpenMicInputPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicInput, openmic_input, OPENMIC_TYPE_NODE);

static void openmic_input_class_init(OpenMicInputClass* klass) {}

static void openmic_input_init(OpenMicInput* self) {
	OpenMicInputPrivate* priv = openmic_input_get_instance_private(self);
	priv->src = gst_element_factory_make("autoaudiosrc", NULL);
	g_object_set(G_OBJECT(self), "element", priv->src, NULL);
}
