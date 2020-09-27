#include <OpenMic/context.h>

typedef struct _OpenMicContextPrivate {
	GPtrArray* modules;
} OpenMicContextPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicContext, openmic_context, G_TYPE_OBJECT);

static void openmic_context_class_init(OpenMicContextClass* klass) {}
static void openmic_context_init(OpenMicContext* self) {}
