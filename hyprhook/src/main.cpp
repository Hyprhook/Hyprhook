#include <hyprlang.hpp>
#include <ostream>
#include <unistd.h>
#define WLR_USE_UNSTABLE

#include <hyprland/src/plugins/PluginAPI.hpp>
#define private public
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/Timer.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#undef private

#include "globals.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

static void onConfigReloaded(void* self, std::any data) {
    for (auto& event : Global::functionsMap) {
        Global::enabledMap[event.first] = !static_cast<std::string>(*Global::eventMap[event.first]).empty();
    }
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    Global::PHANDLE        = handle;
    const std::string HASH = __hyprland_api_get_hash();

    if (HASH != GIT_COMMIT_HASH) {
        const CHyprColor errorColor(1.0f, 0.0f, 0.0f, 1.0f);
        HyprlandAPI::addNotification(Global::PHANDLE, std::format("[{}] Mismatched headers! Can't proceed.", Global::pluginName), errorColor, 5000);
        throw std::runtime_error(std::format("[{}] Version mismatch", Global::pluginName));
    }

    for (auto& elm : Global::functionsMap) {
        const std::string& event = elm.first;
        if (event == "submap") {
            HyprlandAPI::addConfigValue(Global::PHANDLE, std::format("plugin:{}:onSubmap", Global::configPName), Hyprlang::STRING{""});
            Global::eventMap[event] =
                (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(Global::PHANDLE, std::format("plugin:{}:onSubmap", Global::configPName))->getDataStaticPtr();

        } else {
            HyprlandAPI::addConfigValue(Global::PHANDLE, std::format("plugin:{}:{}", Global::configPName, event), Hyprlang::STRING{""});
            Global::eventMap[event] =
                (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(Global::PHANDLE, std::format("plugin:{}:{}", Global::configPName, event))->getDataStaticPtr();
        }
        Global::enabledMap[event] = !static_cast<std::string>(*Global::eventMap[event]).empty();

        Global::hookMap[event] = HyprlandAPI::registerCallbackDynamic(Global::PHANDLE, event, [&](void* self, SCallbackInfo& info, std::any data) {
            if (!Global::enabledMap[event]) {
                return;
            }
            const auto& it = Global::functionsMap.find(event);
            if (it == Global::functionsMap.end()) {
                return;
            }

            const std::string& script   = *Global::eventMap[event];
            const std::string& spawnStr = script + " '" + it->second(data) + "'";
            g_pKeybindManager->spawn(spawnStr);
        });
    }

    static auto P = HyprlandAPI::registerCallbackDynamic(Global::PHANDLE, "configReloaded", [&](void* self, SCallbackInfo& info, std::any data) { onConfigReloaded(self, data); });

    HyprlandAPI::reloadConfig();

    const CHyprColor successColor(0.0f, 1.0f, 0.0f, 1.0f);
    HyprlandAPI::addNotification(Global::PHANDLE, std::format("[{}] Initialized successfully!", Global::pluginName), successColor, 5000);

    return {Global::pluginName, "A hook proxy that lets you run scripts on event trigger", "Moritz Gleissner, Yusuf Duran", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // ...
}
