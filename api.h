#ifndef _API_H_
#define _API_H_

#include <reaper_plugin_functions.h>
#ifdef REAPERAPI_IMPLEMENT
#define REAPER_EXTRA_API_DECL
#else
#define REAPER_EXTRA_API_DECL extern
#endif
#

REAPER_EXTRA_API_DECL MediaTrack * (*BR_TrackAtMouseCursor)(int *contextOut, double *positionOut);

REAPER_EXTRA_API_DECL
void (*BR_GetMouseCursorContext)(char *windowOut, int windowOut_sz, char *segmentOut, int segmentOut_sz, char *detailsOut, int detailsOut_sz);
#endif
