#include <hyprlang.hpp>
#include <ostream>
#include <unistd.h>
#define WLR_USE_UNSTABLE

#include <hyprland/src/plugins/PluginAPI.hpp>
#define private public
#include <hyprland/src/debug/HyprCtl.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#undef private

#include "globals.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

// object to json parser functions
namespace Parser {

    std::string parseEmpty(std::any data) {
        const std::string& ret = "";
        return ret;
    }

    std::string parseSubmap(std::any data) {
        const auto& ret = std::any_cast<std::string>(data);
        return ret;
    }

    std::string parseWindow(std::any data) {
        const auto&        window = std::any_cast<PHLWINDOW>(data);
        const std::string& ret    = CHyprCtl::getWindowData(window, eHyprCtlOutputFormat::FORMAT_JSON);
        return ret;
    }

    std::string parseWorkspace(std::any data) {
        const auto&        workspace = std::any_cast<PHLWORKSPACE>(data);
        const std::string& ret       = CHyprCtl::getWorkspaceData(workspace, eHyprCtlOutputFormat::FORMAT_JSON);
        return ret;
    }

    std::string parseCWorkspace(std::any data) {
        const auto&        workspace = std::any_cast<CWorkspace*>(data);
        const std::string& ret       = parseWorkspace(workspace->m_pSelf.lock());
        return ret;
    }

    std::string parseMonitor(std::any data) {
        const auto&        monitor = std::any_cast<CMonitor*>(data);
        const std::string& ret     = CHyprCtl::getMonitorData(monitor->self.lock(), eHyprCtlOutputFormat::FORMAT_JSON);
        return ret;
    }

    std::string parseVectorWorkspaceMonitor(std::any data) {
        std::string ret = "[";
        const auto& vec = std::any_cast<std::vector<std::any>>(data);
        for (const auto& it : vec) {
            if (it.type() == typeid(PHLWORKSPACE)) {
                ret += parseWorkspace(it);
            } else if (it.type() == typeid(CMonitor)) {
                ret += CHyprCtl::getMonitorData(std::any_cast<CMonitor*>(it)->self.lock(), eHyprCtlOutputFormat::FORMAT_JSON);
            }
            ret += ",";
        }
        if (ret.length() > 1) {
            ret.pop_back();
        }
        ret += "]";
        return ret;
    }

    std::string parseVetorWindowWorkspace(std::any data) {
        std::string ret = "[";
        const auto& vec = std::any_cast<std::vector<std::any>>(data);
        for (const auto& it : vec) {
            if (it.type() == typeid(PHLWINDOW)) {
                ret += parseWindow(it);
            } else if (it.type() == typeid(PHLWORKSPACE)) {
                ret += parseWorkspace(it);
            }
        }
        if (ret.back() == ',') {
            ret.pop_back();
        }
        ret += "]";
        return ret;
    }

    std::string parseVector2D(std::any data) {
        const auto&        vec = std::any_cast<Vector2D>(data);
        const std::string& ret = "{ \"x\": " + std::to_string(vec.x) + ", \"y\": " + std::to_string(vec.y) + " }";
        return ret;
    }

    // state: 0 = released, 1 = pressed
    std::string parseSButtonEvent(std::any data) {
        const auto&        event = std::any_cast<IPointer::SButtonEvent>(data);
        const std::string& ret =
            "{ \"timeMs\": " + std::to_string(event.timeMs) + ", \"button\": " + std::to_string(event.button) + ", \"state\": " + std::to_string(event.state) + " }";
        return ret;
    }

    std::string parseSDownEvent(std::any data) {
        const auto&        event = std::any_cast<ITouch::SDownEvent>(data);
        const std::string& ret =
            "{ \"timeMs\": " + std::to_string(event.timeMs) + ", \"touchID\": " + std::to_string(event.touchID) + ", \"pos\": " + parseVector2D(event.pos) + " }";
        return ret;
    }

    std::string parseSUpEvent(std::any data) {
        const auto&        event = std::any_cast<ITouch::SUpEvent>(data);
        const std::string& ret   = "{ \"timeMs\": " + std::to_string(event.timeMs) + ", \"touchID:\" " + std::to_string(event.touchID) + " }";
        return ret;
    }
} // namespace Parser

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
