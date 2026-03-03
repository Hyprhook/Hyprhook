#include <hyprlang.hpp>
#include <ostream>
#include <unistd.h>
#define WLR_USE_UNSTABLE

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/Compositor.hpp>

#include "globals.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

static void onConfigReloaded() {
    for (auto& event : Global::functionsMap) {
        Global::enabledMap[event.first] = !static_cast<std::string>(*Global::eventMap[event.first]).empty();
    }
}

template <typename T>
static void executeHook(const std::string& event, T data) {
    if (!Global::hyprlandReady) {
        return;
    }
    const CHyprColor errorColor(1.0f, 0.0f, 0.0f, 1.0f);
    if (!Global::enabledMap[event]) {
        return;
    }
    const auto& it = Global::functionsMap.find(event);

    if (it == Global::functionsMap.end()) {
        return;
    }

    const std::string& script = *Global::eventMap[event];

    try {
        std::any anyData = data;
        const std::string& spawnStr = script + " '" + it->second(anyData) + "'";
        HyprlandAPI::invokeHyprctlCommand("dispatch", "exec " + spawnStr);
    } catch (const std::bad_any_cast&) {
        HyprlandAPI::addNotification(Global::PHANDLE, std::format("[{}] Bad any cast for '{}'. The HyprlandAPI might have changed", Global::pluginName, event), errorColor, 5000);
    }
}

static void executeHookEmpty(const std::string& event) {
    if (!Global::hyprlandReady) {
        return;
    }
    const CHyprColor errorColor(1.0f, 0.0f, 0.0f, 1.0f);
    if (!Global::enabledMap[event]) {
        return;
    }
    const auto& it = Global::functionsMap.find(event);

    if (it == Global::functionsMap.end()) {
        return;
    }

    const std::string& script = *Global::eventMap[event];

    try {
        std::any anyData = nullptr;
        const std::string& spawnStr = script + " '" + it->second(anyData) + "'";
        HyprlandAPI::invokeHyprctlCommand("dispatch", "exec " + spawnStr);
    } catch (const std::bad_any_cast&) {
        HyprlandAPI::addNotification(Global::PHANDLE, std::format("[{}] Bad any cast for '{}'. The HyprlandAPI might have changed", Global::pluginName, event), errorColor, 5000);
    }
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    Global::PHANDLE        = handle;
    const std::string HASH = __hyprland_api_get_hash();

    if (HASH != __hyprland_api_get_client_hash()) {
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

        } else if (event == "workspace") {
            HyprlandAPI::addConfigValue(Global::PHANDLE, std::format("plugin:{}:onWorkspace", Global::configPName), Hyprlang::STRING{""});
            Global::eventMap[event] =
                (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(Global::PHANDLE, std::format("plugin:{}:onWorkspace", Global::configPName))->getDataStaticPtr();

        } else {
            HyprlandAPI::addConfigValue(Global::PHANDLE, std::format("plugin:{}:{}", Global::configPName, event), Hyprlang::STRING{""});
            Global::eventMap[event] =
                (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(Global::PHANDLE, std::format("plugin:{}:{}", Global::configPName, event))->getDataStaticPtr();
        }
        Global::enabledMap[event] = !static_cast<std::string>(*Global::eventMap[event]).empty();
    }

    // Register event listeners using Event::bus()
    // Listen for the ready event to know when Hyprland is fully initialized
    static auto ready = Event::bus()->m_events.ready.listen([]() {
        Global::hyprlandReady = true;
    });
    
    // If there are already monitors (meaning we're past initialization), mark as ready immediately
    // This handles the case where the plugin is loaded dynamically after Hyprland has started
    if (g_pCompositor && !g_pCompositor->m_monitors.empty()) {
        Global::hyprlandReady = true;
    }

    static auto activeWindow = Event::bus()->m_events.window.active.listen([](PHLWINDOW window, Desktop::eFocusReason reason) {
        executeHook("activeWindow", window);
    });

    static auto openWindow = Event::bus()->m_events.window.open.listen([](PHLWINDOW window) {
        executeHook("openWindow", window);
    });

    static auto closeWindow = Event::bus()->m_events.window.close.listen([](PHLWINDOW window) {
        executeHook("closeWindow", window);
    });

    static auto windowTitle = Event::bus()->m_events.window.title.listen([](PHLWINDOW window) {
        executeHook("windowtitle", window);
    });

    static auto urgent = Event::bus()->m_events.window.urgent.listen([](PHLWINDOW window) {
        executeHook("urgent", window);
    });

    static auto fullscreen = Event::bus()->m_events.window.fullscreen.listen([](PHLWINDOW window) {
        executeHook("fullscreen", window);
    });

    static auto windowUpdateRules = Event::bus()->m_events.window.updateRules.listen([](PHLWINDOW window) {
        executeHook("windowUpdateRules", window);
    });

    static auto moveWindow = Event::bus()->m_events.window.moveToWorkspace.listen([](PHLWINDOW window, PHLWORKSPACE workspace) {
        executeHook("moveWindow", std::vector<std::any>{window, workspace});
    });

    static auto pin = Event::bus()->m_events.window.pin.listen([](PHLWINDOW window) {
        executeHook("pin", window);
    });

    static auto workspace = Event::bus()->m_events.workspace.active.listen([](PHLWORKSPACE workspace) {
        executeHook("workspace", workspace);
    });

    static auto createWorkspace = Event::bus()->m_events.workspace.created.listen([](PHLWORKSPACEREF workspaceRef) {
        auto workspace = workspaceRef.lock();
        if (workspace)
            executeHook("createWorkspace", workspace.get());
    });

    static auto destroyWorkspace = Event::bus()->m_events.workspace.removed.listen([](PHLWORKSPACEREF workspaceRef) {
        auto workspace = workspaceRef.lock();
        if (workspace)
            executeHook("destroyWorkspace", workspace.get());
    });

    static auto moveWorkspace = Event::bus()->m_events.workspace.moveToMonitor.listen([](PHLWORKSPACE workspace, PHLMONITOR monitor) {
        executeHook("moveWorkspace", std::vector<std::any>{workspace, monitor});
    });

    static auto monitorAdded = Event::bus()->m_events.monitor.added.listen([](PHLMONITOR monitor) {
        executeHook("monitorAdded", monitor);
    });

    static auto monitorRemoved = Event::bus()->m_events.monitor.removed.listen([](PHLMONITOR monitor) {
        executeHook("monitorRemoved", monitor);
    });

    static auto focusedMon = Event::bus()->m_events.monitor.focused.listen([](PHLMONITOR monitor) {
        executeHook("focusedMon", monitor);
    });

    static auto preRender = Event::bus()->m_events.render.pre.listen([](PHLMONITOR monitor) {
        executeHook("preRender", monitor);
    });

    static auto render = Event::bus()->m_events.render.stage.listen([](eRenderStage stage) {
        executeHook("render", stage);
    });

    static auto configReloaded = Event::bus()->m_events.config.reloaded.listen([]() {
        onConfigReloaded();
    });

    static auto preConfigReload = Event::bus()->m_events.config.preReload.listen([]() {
        executeHookEmpty("preConfigReload");
    });

    static auto submap = Event::bus()->m_events.keybinds.submap.listen([](const std::string& submapName) {
        executeHook("submap", submapName);
    });

    static auto mouseMove = Event::bus()->m_events.input.mouse.move.listen([](Vector2D pos, Event::SCallbackInfo&) {
        executeHook("mouseMove", pos);
    });

    static auto mouseButton = Event::bus()->m_events.input.mouse.button.listen([](IPointer::SButtonEvent event, Event::SCallbackInfo&) {
        executeHook("mouseButton", event);
    });

    static auto mouseAxis = Event::bus()->m_events.input.mouse.axis.listen([](IPointer::SAxisEvent event, Event::SCallbackInfo&) {
        executeHook("mouseAxis", event);
    });

    static auto keyPress = Event::bus()->m_events.input.keyboard.key.listen([](IKeyboard::SKeyEvent event, Event::SCallbackInfo&) {
        executeHook("keyPress", event);
    });

    static auto keyboardFocus = Event::bus()->m_events.input.keyboard.focus.listen([](SP<CWLSurfaceResource> surface) {
        executeHook("keyboardFocus", surface);
    });

    static auto activeLayout = Event::bus()->m_events.input.keyboard.layout.listen([](SP<IKeyboard> keyboard, const std::string& layout) {
        executeHook("activeLayout", std::vector<std::any>{keyboard, layout});
    });

    static auto touchDown = Event::bus()->m_events.input.touch.down.listen([](ITouch::SDownEvent event, Event::SCallbackInfo&) {
        executeHook("touchDown", event);
    });

    static auto touchUp = Event::bus()->m_events.input.touch.up.listen([](ITouch::SUpEvent event, Event::SCallbackInfo&) {
        executeHook("touchUp", event);
    });

    static auto touchMove = Event::bus()->m_events.input.touch.motion.listen([](ITouch::SMotionEvent event, Event::SCallbackInfo&) {
        executeHook("touchMove", event);
    });

    static auto swipeBegin = Event::bus()->m_events.gesture.swipe.begin.listen([](IPointer::SSwipeBeginEvent event, Event::SCallbackInfo&) {
        executeHook("swipeBegin", event);
    });

    static auto swipeUpdate = Event::bus()->m_events.gesture.swipe.update.listen([](IPointer::SSwipeUpdateEvent event, Event::SCallbackInfo&) {
        executeHook("swipeUpdate", event);
    });

    static auto swipeEnd = Event::bus()->m_events.gesture.swipe.end.listen([](IPointer::SSwipeEndEvent event, Event::SCallbackInfo&) {
        executeHook("swipeEnd", event);
    });

    HyprlandAPI::reloadConfig();

    return {Global::pluginName, "A hook proxy that lets you run scripts on event trigger", "Moritz Gleissner, Yusuf Duran", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // ...
}
