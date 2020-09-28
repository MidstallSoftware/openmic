#include <OpenMic/node.h>

typedef struct _OpenMicNodePrivate {} OpenMicNodePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(OpenMicNode, openmic_node, G_TYPE_OBJECT);

static void openmic_node_class_init(OpenMicNodeClass* klass) {}

static void openmic_node_init(OpenMicNode* self) {}
