#include <OpenMic/nodes/file-player.h>

typedef struct _OpenMicFilePlayerPrivate {
} OpenMicFilePlayerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OpenMicFilePlayer, openmic_file_player, OPENMIC_TYPE_NODE);

static void openmic_file_player_class_init(OpenMicFilePlayerClass* klass) {}

static void openmic_file_player_init(OpenMicFilePlayer* self) {
	OpenMicFilePlayerPrivate* priv = openmic_file_player_get_instance_private(self);
}
