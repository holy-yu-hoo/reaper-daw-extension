#include <reaper_plugin.h>
#include "core/api_load.h"
#include "commands/commands.h"
#include "core/api.h"

reaper_plugin_info_t*g_rec=nullptr;

extern "C" REAPER_PLUGIN_DLL_EXPORT int ReaperPluginEntry(
	HINSTANCE hInst, reaper_plugin_info_t* rec) {
	if (!rec || rec->caller_version != REAPER_PLUGIN_VERSION) {
		return 0;
	}

	g_rec = rec;
	if (!api_load()) {
		return 0;
	}

	if (!init_actions()) {
		return 0;
	}
	rec->Register("hookcommand2", static_cast<void*>(&action_dispatch_hook));

	#if defined(_DEBUG)
	ShowConsoleMsg("Extension loaded successfully.\n");
	#endif

	return 1;
}