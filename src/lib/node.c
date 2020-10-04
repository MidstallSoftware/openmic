#include <OpenMic/node.h>
#include <AL/al.h>
#include <portaudio.h>

enum {
	PROP_0,
	PROP_CONTEXT,
	PROP_POS,
	N_PROPS
};

typedef struct _OpenMicNodePrivate {
	OpenMicContext* ctx;
	GNode* gnode;
	PaStream* stream;
	ALuint albuff;
	ALuint alsrc;
	float pos[3];
} OpenMicNodePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicNode, openmic_node, G_TYPE_OBJECT);

static GParamSpec* obj_props[N_PROPS] = { NULL };

static gboolean openmic_node_sync_stream_children(GNode* node, gpointer data) {
	return !openmic_node_sync_stream(node->data, data);
}

static void openmic_node_constructed(GObject* obj) {
	OpenMicNode* self = OPENMIC_NODE(obj);
	OpenMicNodeClass* klass = OPENMIC_NODE_GET_CLASS(self);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);

	gboolean is3d = FALSE;
	g_object_get(G_OBJECT(priv->ctx), "enable-3d", &is3d, NULL);

	OpenMicNodeStreamInfo stream_info = {
		.loop = FALSE
	};
	g_assert(klass->stream_info);
	klass->stream_info(self, &stream_info);

	if (is3d) {
		alGetError();
		alGenBuffers(1, &priv->albuff);
		int err = alGetError();
		if (err != AL_NO_ERROR) {
			g_error("OpenMicNode: failed to generate buffer: (OpenAL) %d", err);
			g_assert(err != AL_NO_ERROR);
		}

		alGetError();
		alGenSources(1, &priv->alsrc);
		err = alGetError();
		if (err != AL_NO_ERROR) {
			g_error("OpenMicNode: failed to generate source: (OpenAL) %d", err);
			g_assert(err != AL_NO_ERROR);
		}

		alSourcei(priv->alsrc, AL_BUFFER, priv->albuff);
		alSourcei(priv->alsrc, AL_LOOPING, stream_info.loop);
		alSourcefv(priv->alsrc, AL_POSITION, priv->pos);
	} else {
	}
}

static void openmic_node_dispose(GObject* obj) {
	G_OBJECT_CLASS(openmic_node_parent_class)->dispose(obj);
}

static void openmic_node_finalize(GObject* obj) {
	OpenMicNode* self = OPENMIC_NODE(obj);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);

	g_node_destroy(priv->gnode);

	G_OBJECT_CLASS(openmic_node_parent_class)->finalize(obj);
}

static void openmic_node_set_property(GObject* obj, guint propid, const GValue* value, GParamSpec* pspec) {
	OpenMicNode* self = OPENMIC_NODE(obj);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			priv->ctx = g_value_get_object(value);
			break;
		case PROP_POS:
			{
				float* val = g_value_get_boxed(value);
				priv->pos[0] = val[0];
				priv->pos[1] = val[1];
				priv->pos[2] = val[2];

				gboolean is3d = FALSE;
				g_object_get(G_OBJECT(priv->ctx), "enable-3d", &is3d, NULL);
				if (!is3d) g_warning("Setting position for a node with 3-D disabled in this context");
				else {
					alSourcefv(priv->alsrc, AL_POSITION, priv->pos);
				}
			}
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_node_get_property(GObject* obj, guint propid, GValue* value, GParamSpec* pspec) {
	OpenMicNode* self = OPENMIC_NODE(obj);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	switch (propid) {
		case PROP_CONTEXT:
			g_value_set_object(value, priv->ctx);
			break;
		case PROP_POS:
			g_value_set_boxed(value, priv->pos);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, propid, pspec);
			break;
	}
}

static void openmic_node_class_init(OpenMicNodeClass* klass) {
	GObjectClass* object_class = G_OBJECT_CLASS(klass);

	object_class->constructed = openmic_node_constructed;
	object_class->dispose = openmic_node_dispose;
	object_class->finalize = openmic_node_finalize;
	object_class->set_property = openmic_node_set_property;
	object_class->get_property = openmic_node_get_property;

	obj_props[PROP_CONTEXT] = g_param_spec_object("context", "Context", "OpenMic context instance", OPENMIC_TYPE_CONTEXT, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	obj_props[PROP_POS] = g_param_spec_value_array("position", "Position", "OpenMic Node position in 3-D space", g_param_spec_float("position", "", "", -1.0f, 1.0f, 0.0f, G_PARAM_READWRITE), G_PARAM_READWRITE);
	g_object_class_install_properties(object_class, N_PROPS, obj_props);
}

static void openmic_node_init(OpenMicNode* self) {
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	priv->gnode = g_node_new(self);
}

OpenMicNode* openmic_node_new(OpenMicContext* ctx, const gchar* name) {
	GType type = g_type_from_name(name);
	if (type == G_TYPE_NONE) return NULL;
	OpenMicNode* node = OPENMIC_NODE(g_object_new(type, "context", ctx, NULL));
	g_assert(G_TYPE_CHECK_INSTANCE_TYPE(node, OPENMIC_TYPE_NODE));
	return node;
}

OpenMicContext* openmic_node_get_context(OpenMicNode* self) {
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	return priv->ctx;
}

void openmic_node_attach(OpenMicNode* self, gint i, OpenMicNode* other) {
	g_assert(G_TYPE_CHECK_INSTANCE_TYPE(self, OPENMIC_TYPE_NODE));
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);

	OpenMicNodeClass* klass = OPENMIC_NODE_GET_CLASS(self);
	OpenMicNodeClass* other_class = OPENMIC_NODE_GET_CLASS(other);

	g_assert(klass);
	g_assert(other_class);

	g_node_insert(priv->gnode, i, openmic_node_get_gnode(self));
}
void openmic_node_remove(OpenMicNode* self, OpenMicNode* other) {
	g_assert(G_TYPE_CHECK_INSTANCE_TYPE(self, OPENMIC_TYPE_NODE));

	g_node_unlink(openmic_node_get_gnode(other));
}

GNode* openmic_node_get_gnode(OpenMicNode* self) {
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	return priv->gnode;
}

gboolean openmic_node_sync_stream(OpenMicNode* self, GError** error) {
	OpenMicNodeClass* klass = OPENMIC_NODE_GET_CLASS(self);
	OpenMicNodePrivate* priv = openmic_node_get_instance_private(self);
	if (klass->read) {
		uint16_t* buff;
		size_t len = klass->read(self, &buff, error);
		if (!len) return FALSE;

		OpenMicNodeStreamInfo stream_info;
		g_assert(klass->stream_info);
		klass->stream_info(self, &stream_info);

		gboolean is3d = FALSE;
		g_object_get(G_OBJECT(priv->ctx), "enable-3d", &is3d, NULL);

		if (is3d) {
			alGetError();
			// TODO: get format from stream info
			alBufferData(priv->albuff, AL_FORMAT_STEREO16, buff, len, stream_info.sample_rate);
			int err = alGetError();
			if (err != AL_NO_ERROR) {
				g_set_error(error, 0, err, "OpenMicNode: failed to write stream: (OpenAL) %d", err);
				return FALSE;
			}
		} else {
			PaError err = Pa_WriteStream(priv->stream, buff, len);
			if (err != paNoError) {
				g_set_error(error, 0, err, "OpenMicNode: failed to write stream: (PortAudio) %s", Pa_GetErrorText(err));
				return FALSE;
			}
		}
	}
	g_node_traverse(priv->gnode, G_LEVEL_ORDER, G_TRAVERSE_ALL, -1, openmic_node_sync_stream_children, error);
	return error == NULL;
}
