#pragma once
#include <reaper_plugin_functions.h>
#include "yh_api.h"

extern MediaTrack * (*BR_TrackAtMouseCursor)(int *contextOut, double *positionOut);

extern void (*BR_GetMouseCursorContext)(char *windowOut, int windowOut_sz, char *segmentOut, int segmentOut_sz, char *detailsOut, int detailsOut_sz);
