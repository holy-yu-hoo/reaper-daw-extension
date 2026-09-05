#include "api.h"
#include "item.h"


void delete_items_ripple_per_track(COMMAND_T* cmd) {
    int size = 0;
    int offset = projectconfig_var_getoffs("projripedit", &size);
    void* addres = projectconfig_var_addr(nullptr, offset);
    int* addr = static_cast<int*>(addres);
    int rip_mode = *addr;
    *addr = 1;
    Main_OnCommandEx(40006, 0, nullptr); // Item: Remove items
    *addr = rip_mode;
}

void delete_items_ripple_all_track(COMMAND_T* cmd) {
    int size = 0;
    int offset = projectconfig_var_getoffs("projripedit", &size);
    void* addres = projectconfig_var_addr(nullptr, offset);
    int* addr = static_cast<int*>(addres);
    int rip_mode = *addr;
    *addr = 2;
    Main_OnCommandEx(40006, 0, nullptr); // Item: Remove items
    *addr = rip_mode;
}