#include <hyprlang.hpp>
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

static void hookCaller(Hyprlang::STRING const& hook, std::vector<std::string> const& args = {}) {
    HyprlandAPI::addNotification(PHANDLE, std::format("[hypr-which-key] Gonna execute {} with args: {}!", hook, ""), CColor{0.2, 1.0, 0.2, 1.0}, 5000);
    g_pKeybindManager->spawn(hook);
}

std::vector<std::string> events = {
    "submap",
    "activeWindow",
    "workspace",
};

static std::unordered_map<std::string, Hyprlang::STRING const*>          eventMap = {};
static std::unordered_map<std::string, CSharedPointer<HOOK_CALLBACK_FN>> hookMap  = {};

APICALL EXPORT PLUGIN_DESCRIPTION_INFO                                   PLUGIN_INIT(HANDLE handle) {
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
        hookMap[event] = HyprlandAPI::registerCallbackDynamic(PHANDLE, event, [&](void* self, SCallbackInfo& info, std::any data) { hookCaller(*eventMap[event]); });
    }

    HyprlandAPI::reloadConfig();

    HyprlandAPI::addNotification(PHANDLE, "[hypr-which-key] Initialized successfully!", CColor{0.2, 1.0, 0.2, 1.0}, 5000);

    return {"hypr-which-key", "An amazing plugin that is going to change the world!", "Moritz Gleissner, Yusuf Duran", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // ...
}
