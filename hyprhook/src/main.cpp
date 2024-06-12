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

static std::unordered_map<std::string, std::function<std::string(std::any)>> functionsMap = {{"activeWindow", [](std::any data) { return ""; }},
                                                                                             {"keyboardFocus", [](std::any data) { return ""; }},
                                                                                             {"moveWorkspace", [](std::any data) { return ""; }},
                                                                                             {"focusedMon", [](std::any data) { return ""; }}, //
                                                                                             {"moveWindow", [](std::any data) { return ""; }},
                                                                                             {"openLayer", [](std::any data) { return ""; }},
                                                                                             {"closeLayer", [](std::any data) { return ""; }},
                                                                                             {"openWindow", [](std::any data) { return ""; }},
                                                                                             {"closeWindow", [](std::any data) { return ""; }},
                                                                                             {"windowUpdateRules", [](std::any data) { return ""; }},
                                                                                             {"urgent", [](std::any data) { return ""; }},
                                                                                             {"minimize", [](std::any data) { return ""; }},
                                                                                             {"monitorAdded", [](std::any data) { return ""; }},
                                                                                             {"monitorRemoved", [](std::any data) { return ""; }},
                                                                                             {"createWorkspace", [](std::any data) { return ""; }},
                                                                                             {"destroyWorkspace", [](std::any data) { return ""; }},
                                                                                             {"fullscreen", [](std::any data) { return ""; }},
                                                                                             {"changeFloatingMode", [](std::any data) { return ""; }},
                                                                                             {"workspace", [](std::any data) { return ""; }},
                                                                                             {"submap", [](std::any data) { return std::any_cast<std::string>(data); }},
                                                                                             {"mouseMove", [](std::any data) { return ""; }},
                                                                                             {"mouseButton", [](std::any data) { return ""; }},
                                                                                             {"mouseAxis", [](std::any data) { return ""; }},
                                                                                             {"touchDown", [](std::any data) { return ""; }},
                                                                                             {"touchUp", [](std::any data) { return ""; }},
                                                                                             {"touchMove", [](std::any data) { return ""; }},
                                                                                             {"activeLayout", [](std::any data) { return ""; }},
                                                                                             {"preRender", [](std::any data) { return ""; }},
                                                                                             {"screencast", [](std::any data) { return ""; }},
                                                                                             {"render", [](std::any data) { return ""; }},
                                                                                             {"windowtitle", [](std::any data) { return ""; }},
                                                                                             {"configReloaded", [](std::any data) { return ""; }},
                                                                                             {"preConfigReload", [](std::any data) { return ""; }},
                                                                                             {"keyPress", [](std::any data) { return ""; }},
                                                                                             {"pin", [](std::any data) { return ""; }},
                                                                                             {"swipeBegin", [](std::any data) { return ""; }},
                                                                                             {"swipeUpdate", [](std::any data) { return ""; }},
                                                                                             {"swipeEnd", [](std::any data) { return ""; }}};

static std::vector<std::string>                                              events;
static std::unordered_map<std::string, Hyprlang::STRING const*>              eventMap;
static std::unordered_map<std::string, CSharedPointer<HOOK_CALLBACK_FN>>     hookMap;

// nuhu

static void onConfigReloaded(void* self, std::any data) {
    HyprlandAPI::addNotification(PHANDLE, "[hypr-which-key] config reoaded ", CColor{0.2, 1.0, 0.2, 1.0}, 5000);
    for (auto& event : events) {
        if ((std::string)*eventMap[event] == "") {
            hookMap.erase(event);
        }
    }
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    for (const auto& it : functionsMap) {
        events.push_back(it.first);
    }

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[hypr-which-key] Mismatched headers! Can't proceed.", CColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[hypr-which-key] Version mismatch");
    }

    for (auto& event : events) {
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprhook:" + event, Hyprlang::STRING{""});
        eventMap[event] = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hyprhook:" + event)->getDataStaticPtr();

        hookMap[event] = HyprlandAPI::registerCallbackDynamic(PHANDLE, event, [&](void* self, SCallbackInfo& info, std::any data) {
            if ((std::string)*eventMap[event] == "") {
                return;
            }
            const auto& it = functionsMap.find(event);
            if (it == functionsMap.end()) {
                return;
            }

            g_pKeybindManager->spawn(std::format("{} {}", *eventMap[event], functionsMap[event](data)));
        });
    }

    static auto P = HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void* self, SCallbackInfo& info, std::any data) { onConfigReloaded(self, data); });

    HyprlandAPI::reloadConfig();

    HyprlandAPI::addNotification(PHANDLE, "[Hyprhook] Initialized successfully!", CColor{0.2, 1.0, 0.2, 1.0}, 5000);

    return {"Hyprhook", "A hook proxy that lets you run scripts on event trigger", "Moritz Gleissner, Yusuf Duran", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // ...
}
