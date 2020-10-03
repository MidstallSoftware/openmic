#include <OpenMic/nodes/output.h>

typedef struct _OpenMicOutputPrivate {
	GstElement* elem;
} OpenMicOutputPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicOutput, openmic_output, OPENMIC_TYPE_NODE);

static void openmic_output_class_init(OpenMicOutputClass* klass) {
}

static void openmic_output_init(OpenMicOutput* self) {
	OpenMicOutputPrivate* priv = openmic_output_get_instance_private(self);
	priv->elem = gst_element_factory_make("autoaudiosink", NULL);
	g_object_set(G_OBJECT(self), "element", priv->elem, NULL);
}
