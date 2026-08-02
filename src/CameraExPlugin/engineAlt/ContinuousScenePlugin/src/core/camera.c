#pragma bank 255

// Camera Ex Plugin - stock camera.c override, ContinuousScenePlugin variant.
//
// Same as the base variant but with the signed camera coordinates and the
// is_transitioning_scene guard that ContinuousScenePlugin requires.

#include "camera.h"
#include "camera_ex.h"

#include "actor.h"
#include "continuous_scene.h"

INT16 camera_x;
INT16 camera_y;
INT16 camera_clamp_x;
INT16 camera_clamp_y;
BYTE camera_offset_x;
BYTE camera_offset_y;
BYTE camera_deadzone_x;
BYTE camera_deadzone_y;
UBYTE camera_settings;

void camera_init(void) BANKED {
    camera_settings = CAMERA_LOCK_FLAG;
    camera_x = camera_y = 0;
    camera_offset_x = camera_offset_y = 0;
    camera_clamp_x = camera_clamp_y = 0;
    camera_reset();
}

void camera_update(void) BANKED {
    if (is_transitioning_scene) {
        return;
    }
    camera_ex_update();
}
