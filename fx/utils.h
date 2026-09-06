#pragma once

#define REC_FX 0x1000000
#define CONT_FX 0x2000000

#define IS_REC_FX(fx) (((fx) & REC_FX)!=0)
#define IS_CONT_FX(fx) (((fx) & CONT_FX)!=0)

#define MAKE_FX_REC(fx) ((fx) | REC_FX)
#define MAKE_FX_CONT(fx) ((fx) | CONT_FX)

#define MAKE_FX_NOT_REC(fx) ((fx) & ~REC_FX)
#define MAKE_FX_NOT_CONT(fx) ((fx) & CONT_FX)

#define MAKE_FX_CLEAN(fx) ((fx) & ~(REC_FX | CONT_FX)) // delete rec and cont bits
#include <api.h>


int get_selected_fx(MediaTrack* track, bool rec = false);

int get_selected_fx(MediaItem_Take* take);
