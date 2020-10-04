#pragma once

#include <OpenMic/node.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OPENMIC_TYPE_FILE_PLAYER (openmic_file_player_get_type())
G_DECLARE_FINAL_TYPE(OpenMicFilePlayer, openmic_file_player, OPENMIC, FILE_PLAYER, OpenMicNode);

struct _OpenMicFilePlayer {
	OpenMicNode parent_instance;
};

G_END_DECLS
