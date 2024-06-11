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

const std::vector<std::string> events = {
    "activeWindow",    "keyboardFocus",      "moveWorkspace", "focusedMon", "moveWindow",   "openLayer",      "closeLayer",      "openWindow",
    "closeWindow",     "windowUpdateRules",  "urgent",        "minimize",   "monitorAdded", "monitorRemoved", "createWorkspace", "destroyWorkspace",
    "fullscreen",      "changeFloatingMode", "workspace",     "submap",     "mouseMove",    "mouseButton",    "mouseAxis",       "touchDown",
    "touchUp",         "touchMove",          "activeLayout",  "preRender",  "screencast",   "render",         "windowtitle",     "configReloaded",
    "preConfigReload", "keyPress",           "pin",           "swipeBegin", "swipeUpdate",  "swipeEnd"};

static std::unordered_map<std::string, Hyprlang::STRING const*>          eventMap = {};
static std::unordered_map<std::string, CSharedPointer<HOOK_CALLBACK_FN>> hookMap  = {};

// nuhu

static void hookCaller(Hyprlang::STRING const& hook, std::vector<std::string> const& args = {}) {
    std::string argsStr;
    for (const std::string& arg : args) {
        argsStr += " " + arg;
    }
    // HyprlandAPI::addNotification(PHANDLE, std::format("[hypr-which-key] Gonna execute {} with args: {}!", hook, argsStr), CColor{0.2, 1.0, 0.2, 1.0}, 5000);
    g_pKeybindManager->spawn(hook + argsStr);
}

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
            // HyprlandAPI::addNotification(PHANDLE, "[hypr-which-key] hook run ", CColor{1.0, 0.2, 0.2, 1.0}, 5000);
            std::vector<std::string> args = {};
            if (event == "submap") {
                const auto submap = std::any_cast<std::string>(data);
                args.push_back(submap);
            }
            hookCaller(*eventMap[event], args);
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
