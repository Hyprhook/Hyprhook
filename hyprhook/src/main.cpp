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
static std::string parseWindow(std::any data) { //tested
    const auto&        window = std::any_cast<PHLWINDOW>(data);
    const std::string& ret    = getWindowData(window, eHyprCtlOutputFormat::FORMAT_JSON);
    return ret;
}

static std::string parseWorkspace(std::any data) {
    const auto&        workspace = std::any_cast<PHLWORKSPACE>(data);
    const std::string& ret       = getWorkspaceData(workspace, eHyprCtlOutputFormat::FORMAT_JSON);
    return ret;
}

static std::string parceCWorkspace(std::any data) {
    const auto&        workspace = std::any_cast<CWorkspace*>(data);
    const std::string& ret       = parseWorkspace(workspace->m_pSelf.lock());
    return ret;
}

static std::string parseMonitor(std::any data) {
    const auto&        monitor = std::any_cast<CMonitor*>(data);
    const std::string& ret     = getMonitorData(monitor->self.lock(), eHyprCtlOutputFormat::FORMAT_JSON);
    return ret;
}

static std::string parseVectorWorkspaceMonitor(std::any data) {
    std::string ret = "[";
    const auto& vec = std::any_cast<std::vector<std::any>>(data);
    for (const auto& it : vec) {
        if (it.type() == typeid(PHLWORKSPACE)) {
            ret += parseWorkspace(it);
        } else if (it.type() == typeid(CMonitor)) {
            ret += getMonitorData(std::any_cast<CMonitor*>(it)->self.lock(), eHyprCtlOutputFormat::FORMAT_JSON);
        }
        ret += ",";
    }
    if (ret.length() > 1) {
        ret.pop_back();
    }
    ret += "]";
    return ret;
}

static std::string parseVetorWindowWorkspace(std::any data) { //tested
    std::string ret = "[";
    std::cout << "before cast" << std::endl;
    const auto& vec = std::any_cast<std::vector<std::any>>(data);
    std::cout << "after cast" << std::endl;
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

static std::string parseVector2D(std::any data) {
    const auto&       vec = std::any_cast<Vector2D>(data);
    const std::string ret = "{ \"x\": " + std::to_string(vec.x) + ", \"y\": " + std::to_string(vec.y) + "}";
    return ret;
}

// state: 0 = released, 1 = pressed
static std::string parseSButtonEvent(std::any data) {
    const auto& event = std::any_cast<IPointer::SButtonEvent>(data);
    const auto& ret = "{ \"timeMs\": " + std::to_string(event.timeMs) + ", \"button\": " + std::to_string(event.button) + ", \"state\": " + std::to_string(event.state) + "}";
    return ret;
}

static void onConfigReloaded(void* self, std::any data) {
    HyprlandAPI::addNotification(PHANDLE, "[Hyprhook] config reoaded ", CColor{0.2, 1.0, 0.2, 1.0}, 5000);
    for (auto& event : events) {
        enabledMap[event] = (std::string)*eventMap[event] == "" ? false : true;
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
        HyprlandAPI::addNotification(PHANDLE, "[Hyprhook] Mismatched headers! Can't proceed.", CColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[Hyprhook] Version mismatch");
    }

    for (auto& event : events) {
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprhook:" + event, Hyprlang::STRING{""});
        eventMap[event] = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hyprhook:" + event)->getDataStaticPtr();

        enabledMap[event] = (std::string)*eventMap[event] == "" ? false : true;

        hookMap[event] = HyprlandAPI::registerCallbackDynamic(PHANDLE, event, [&](void* self, SCallbackInfo& info, std::any data) {
            if (!enabledMap[event]) {
                return;
            }
            const auto& it = functionsMap.find(event);
            if (it == functionsMap.end()) {
                return;
            }

            g_pKeybindManager->spawn(std::format("{} {}", *eventMap[event], "\"" + functionsMap[event](data) + "\""));
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
