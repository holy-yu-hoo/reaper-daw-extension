#include "api.h"
#include "commands.h"


static reaper_plugin_info_t* g_rec = nullptr;

static std::vector<COMMAND_T> g_commands = {};

bool register_commands(std::vector<COMMAND_T> &commands) {
	static custom_action_register_t car{0};
	for (auto &command: commands) {
		car.idStr = command.id;
		car.name = command.name;
		car.uniqueSectionId = command.unique_section_id;
		command.cmd_id = plugin_register("custom_action", static_cast<void*>(&car));
		if (!command.cmd_id) return false;
		g_commands.push_back(command);
	}
	return true;
}

bool unregister_commands(std::vector<COMMAND_T> &commands) {
	static custom_action_register_t car{0};
	for (auto &command: commands) {
		const int registered_cmd_id = command.cmd_id;
		car.idStr = command.id;
		car.name = command.name;
		car.uniqueSectionId = command.unique_section_id;
		const int ok = plugin_register("-custom_action", static_cast<void*>(&car));
		if (!ok) return false;
		g_commands.erase(
			std::remove_if(g_commands.begin(), g_commands.end(), [cmd_id = registered_cmd_id](const COMMAND_T &registered) {
				return registered.cmd_id == cmd_id;
			}),
			g_commands.end()
		);
		command.cmd_id = 0;
	}
	return true;
}


static bool hook_command_proc_2(KbdSectionInfo* sec, int cmd, int val, int val2, int relmode, HWND hwnd) {
	auto act = std::find_if(g_commands.begin(), g_commands.end(), [cmd](const COMMAND_T &command) { return command.cmd_id == cmd; });
	if (act != std::end(g_commands)) {
		if (act->do_command) {
			act->do_command(&*act);
		} else {
			act->on_action(&*act, val, val2, relmode, hwnd);
		}
		return true;
	}

	return false;
}

static void import_extension_api();

static void Init();

static void Exit();

static void Init() {
	g_rec->Register("timer", static_cast<void*>(import_extension_api));
	g_rec->Register("hookcommand2", static_cast<void*>(&hook_command_proc_2));
	g_rec->Register("atexit", static_cast<void*>(Exit));
	if (!commands_init()) {
		Exit();
	}
}

static void Exit() {
	#ifdef _DEBUG
	ShowConsoleMsg("EXIT");
	#endif
	g_rec->Register("-timer", static_cast<void*>(import_extension_api));
	g_rec->Register("-hookcommand2", static_cast<void*>(&hook_command_proc_2));
	g_rec->Register("-atexit", static_cast<void*>(Exit));
	commands_exit();
}

#define SWS_API(x) {if(!(x=static_cast<decltype(x)>(plugin_getapi(#x)))) return Exit();};

static void import_extension_api() {
	plugin_register("-timer", static_cast<void*>(import_extension_api));
	SWS_API(BR_GetMouseCursorContext);
	SWS_API(BR_TrackAtMouseCursor);
	SWS_API(BR_GetMediaTrackGUID);
	SWS_API(BR_GetMediaTrackByGUID);
	SWS_API(BR_GetMediaItemGUID);
	SWS_API(BR_GetMediaItemByGUID);
	SWS_API(BR_GetMediaItemTakeGUID);
	SWS_API(CF_GetTrackFXChainEx);
	SWS_API(CF_GetTakeFXChain);
	SWS_API(CF_GetFocusedFXChain);

}

static bool implement_api() {
	if (REAPERAPI_LoadAPI(g_rec->GetFunc) != 0) return false;
	return true;
}


extern "C" REAPER_PLUGIN_DLL_EXPORT int ReaperPluginEntry(
	HINSTANCE hInst,
	reaper_plugin_info_t* rec) {
	if (!rec || rec->caller_version != REAPER_PLUGIN_VERSION) {
		return 0;
	}

	g_rec = rec;
	if (!implement_api()) {
		return 0;
	}

	Init();

	#if defined(_DEBUG)
	ShowConsoleMsg("Extension loaded successfully.\n");
	#endif

	return 1;
}
