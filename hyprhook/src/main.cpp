#include <hyprlang.hpp>
#include <ostream>
#include <unistd.h>
#define WLR_USE_UNSTABLE

#include <hyprland/src/plugins/PluginAPI.hpp>
#define private public
#include <hyprland/src/managers/KeybindManager.hpp>
#undef private

#include "globals.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

static void onConfigReloaded(void* self, std::any data) {
    for (auto& event : Global::events) {
        Global::enabledMap[event] = !static_cast<std::string>(*Global::eventMap[event]).empty();
    }
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    Global::PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    for (const auto& it : Global::functionsMap) {
        Global::events.push_back(it.first);
    }

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(Global::PHANDLE, "[Hyprhook] Mismatched headers! Can't proceed.", CColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[Hyprhook] Version mismatch");
    }

    for (auto& event : Global::events) {
        HyprlandAPI::addConfigValue(Global::PHANDLE, "plugin:hyprhook:" + event, Hyprlang::STRING{""});
        Global::eventMap[event] = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(Global::PHANDLE, "plugin:hyprhook:" + event)->getDataStaticPtr();

        Global::enabledMap[event] = !static_cast<std::string>(*Global::eventMap[event]).empty();

        Global::hookMap[event] = HyprlandAPI::registerCallbackDynamic(Global::PHANDLE, event, [&](void* self, SCallbackInfo& info, std::any data) {
            if (!Global::enabledMap[event]) {
                return;
            }
            const auto& it = Global::functionsMap.find(event);
            if (it == Global::functionsMap.end()) {
                return;
            }

            g_pKeybindManager->spawn(std::format("{} {}", *Global::eventMap[event], "\"" + Global::functionsMap[event](data) + "\""));
        });
    }

    static auto P = HyprlandAPI::registerCallbackDynamic(Global::PHANDLE, "configReloaded", [&](void* self, SCallbackInfo& info, std::any data) { onConfigReloaded(self, data); });

    HyprlandAPI::reloadConfig();

    HyprlandAPI::addNotification(Global::PHANDLE, "[Hyprhook] Initialized successfully!", CColor{0.2, 1.0, 0.2, 1.0}, 5000);

    return {"Hyprhook", "A hook proxy that lets you run scripts on event trigger", "Moritz Gleissner, Yusuf Duran", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // ...
}
