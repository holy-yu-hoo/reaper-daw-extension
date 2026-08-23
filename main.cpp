#define  REAPERAPI_IMPLEMENT
#include "api.h"
#include "utils.h"
#include "commands/commands.h"


reaper_plugin_info_t *g_rec = nullptr;

std::vector<COMMAND_T> g_commands = {};

bool register_commands(std::vector<COMMAND_T> &commands) {
	static custom_action_register_t car{0};
	for (auto &command: commands) {
		car.idStr = command.id;
		car.name = command.name;
		car.uniqueSectionId = command.unique_section_id;
		command.cmd_id = plugin_register("custom_action", static_cast<void *>(&car));
		if (!command.cmd_id) return false;
		g_commands.push_back(command);
	}
	return true;
}

bool unregister_commands(std::vector<COMMAND_T> &commands) {
	static custom_action_register_t car{0};
	for (auto &command: commands) {
		car.idStr = command.id;
		car.name = command.name;
		car.uniqueSectionId = command.unique_section_id;
		command.cmd_id = plugin_register("-custom_action", static_cast<void *>(&car));
		if (!command.cmd_id) return false;
		g_commands.push_back(command);
	}
	return true;
}


static bool hook_command_proc_2(KbdSectionInfo *sec, int cmd, int val, int val2, int relmode, HWND hwnd) {
	auto act = std::find_if(g_commands.begin(), g_commands.end(), [cmd](COMMAND_T command) { return command.cmd_id == cmd; });
	if (act == std::end(g_commands)) {
		return false;
	}
	act->do_command(&*act);
	return true;
}

static void import_extension_api();

static void Init();

static void Exit();

static void Init() {
	g_rec->Register("timer", static_cast<void *>(import_extension_api));
	g_rec->Register("hookcommand2", static_cast<void *>(&hook_command_proc_2));
	g_rec->Register("atexit", static_cast<void *>(Exit));
	if (!commands_init()) {
		Exit();
	}
}

static void Exit() {
	ShowConsoleMsg("EXIT");
	g_rec->Register("-timer", static_cast<void *>(import_extension_api));
	g_rec->Register("-hookcommand2", static_cast<void *>(&hook_command_proc_2));
	g_rec->Register("-atexit", static_cast<void *>(Exit));
	commands_exit();
}

#define SWS_API(x) {if(!(x=static_cast<decltype(x)>(plugin_getapi(#x)))) return Exit();};

static void import_extension_api() {
	plugin_register("-timer", static_cast<void *>(import_extension_api));
	SWS_API(BR_GetMouseCursorContext);
	SWS_API(BR_TrackAtMouseCursor);

}

static bool implement_api() {
	if (REAPERAPI_LoadAPI(g_rec->GetFunc) > 0) return false;
	return true;
}


extern "C" REAPER_PLUGIN_DLL_EXPORT int ReaperPluginEntry(
	HINSTANCE hInst,
	reaper_plugin_info_t *rec) {
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
