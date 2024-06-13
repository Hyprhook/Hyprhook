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
static std::string parseWindow(std::any data) {
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

static std::unordered_map<std::string, std::function<std::string(std::any)>> functionsMap = {
    {"activeWindow", parseWindow},
    {"keyboardFocus", [](std::any data) { return ""; }},
    {"moveWorkspace", parseVectorWorkspaceMonitor},
    {"focusedMon", parseMonitor},
    {"moveWindow", [](std::any data) { return ""; }}, //std::vectorstd::any{PHLWINDOW, PHLWORKSPACE}
    {"openLayer", [](std::any data) { return ""; }},  //CLayerSurface*
    {"closeLayer", [](std::any data) { return ""; }}, //CLayerSurface*
    {"openWindow", parseWindow},
    {"closeWindow", parseWindow},
    {"windowUpdateRules", parseWindow},
    {"urgent", parseWindow},
    {"minimize", [](std::any data) { return ""; }}, //std::vectorstd::any{PHLWINDOW, int64_t}
    {"monitorAdded", parseMonitor},
    {"monitorRemoved", parseMonitor},
    {"createWorkspace", parceCWorkspace},
    {"destroyWorkspace", parceCWorkspace},
    {"fullscreen", parseWindow},
    {"changeFloatingMode", parseWindow},
    {"workspace", parceCWorkspace},
    {"submap", [](std::any data) { return std::any_cast<std::string>(data); }},
    {"mouseMove", [](std::any data) { return ""; }},    //const Vector2D
    {"mouseButton", [](std::any data) { return ""; }},  //IPointer::SButtonEvent
    {"mouseAxis", [](std::any data) { return ""; }},    //M: event:IPointer::SAxisEvent
    {"touchDown", [](std::any data) { return ""; }},    //ITouch::SDownEvent
    {"touchUp", [](std::any data) { return ""; }},      //ITouch::SUpEvent
    {"touchMove", [](std::any data) { return ""; }},    //ITouch::SMotionEvent
    {"activeLayout", [](std::any data) { return ""; }}, //std::vectorstd::any{SP, std::string}
    {"preRender", parseMonitor},
    {"screencast", [](std::any data) { return ""; }}, //std::vector<uint64_t>{state, framesInHalfSecond, owner}
    {"render", [](std::any data) { return ""; }},     //eRenderStage
    {"windowtitle", parseWindow},
    {"configReloaded", [](std::any data) { return ""; }},  // nullptr
    {"preConfigReload", [](std::any data) { return ""; }}, // nullptr
    {"keyPress", [](std::any data) { return ""; }},        //M: event:IKeyboard::SButtonEvent, keyboard:SP<IKeyboard>
    {"pin", parseWindow},
    {"swipeBegin", [](std::any data) { return ""; }},  //IPointer::SSwipeBeginEvent
    {"swipeUpdate", [](std::any data) { return ""; }}, //IPointer::SSwipeUpdateEvent
    {"swipeEnd", [](std::any data) { return ""; }}};   //IPointer::SSwipeEndEvent

static std::vector<std::string>                                          events;
static std::unordered_map<std::string, Hyprlang::STRING const*>          eventMap;
static std::unordered_map<std::string, CSharedPointer<HOOK_CALLBACK_FN>> hookMap;
static std::unordered_map<std::string, bool>                             enabledMap;

// nuhu

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
