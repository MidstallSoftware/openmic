#include <OpenMic/nodes/output.h>

typedef struct _OpenMicOutputPrivate {
	GstBin* elem;
	GstElement* convert;
	GstElement* output;
} OpenMicOutputPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicOutput, openmic_output, OPENMIC_TYPE_NODE);

static void openmic_output_class_init(OpenMicOutputClass* klass) {
}

static void openmic_output_init(OpenMicOutput* self) {
	OpenMicOutputPrivate* priv = openmic_output_get_instance_private(self);

	priv->elem = GST_BIN(gst_bin_new(NULL));
	priv->output = gst_element_factory_make("autoaudiosink", NULL);
	priv->convert = gst_element_factory_make("audioconvert", NULL);

	gst_bin_add_many(priv->elem, priv->convert, priv->output, NULL);
	gst_element_link(priv->convert, priv->output);

	g_object_set(G_OBJECT(self), "element", priv->elem, NULL);
}
