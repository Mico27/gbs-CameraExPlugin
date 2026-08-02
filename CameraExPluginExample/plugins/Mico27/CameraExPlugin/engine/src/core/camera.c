#pragma bank 255

// Camera Ex Plugin - stock camera.c override.
//
// The stock globals keep their names and types so that every other engine file
// links unchanged. camera_update() delegates to camera_ex_update(), which lives
// in camera_ex.c and is shared verbatim by every engineAlt variant - only this
// file differs between variants.

#include "camera.h"
#include "camera_ex.h"

#include "actor.h"

UINT16 camera_x;
UINT16 camera_y;
UINT16 camera_clamp_x;
UINT16 camera_clamp_y;
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
    camera_ex_update();
}
