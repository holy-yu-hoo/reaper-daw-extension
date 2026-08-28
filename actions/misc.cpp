#include "api.h"
#include "misc.h"

#define zoom_out_hor 1011
#define zoom_in_hor 1012

int g_h_zoom_mode;
int g_v_zoom_mode;

void save_hor_zoom_mode(COMMAND_T *cmd) {

	int sz = 0;
	void *ptr = get_config_var("zoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	g_h_zoom_mode = *(int *) ptr;
}

void restore_hor_zoom_mode(COMMAND_T *cmd) {

	int sz = 0;
	void *ptr = get_config_var("zoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	*(int *) ptr = g_h_zoom_mode;
}


void save_ver_zoom_mode(COMMAND_T *cmd) {

	int sz = 0;
	void *ptr = get_config_var("vzoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	g_v_zoom_mode = *(int *) ptr;
}

void restore_ver_zoom_mode(COMMAND_T *cmd) {

	int sz = 0;
	void *ptr = get_config_var("vzoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	*(int *) ptr = g_v_zoom_mode;
}
