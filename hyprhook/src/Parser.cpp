#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <mutex>
#define WLR_USE_UNSTABLE
#include "Parser.hpp"
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/debug/HyprCtl.hpp>

#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>

namespace Parser {

    std::shared_mutex hyprctlLock;

    static void       trimTrailingComma(std::string& str) {
        if (!str.empty() && str.back() == ',')
            str.pop_back();
    }

    std::string parseEmpty(std::any data) {
        const std::string& ret = "";
        return ret;
    }

    std::string parseSubmap(std::any data) {
        const auto&       submap = std::any_cast<std::string>(data);
        const std::string ret    = "{ \"submap\": \"" + submap + "\" }";
        return ret;
    }

    std::string parseWindow(std::any data) {
        const auto& lock   = std::scoped_lock(hyprctlLock);
        const auto& window = std::any_cast<PHLWINDOW>(data);
        std::string ret    = CHyprCtl::CHyprCtl::getWindowData(window, eHyprCtlOutputFormat::FORMAT_JSON);
        trimTrailingComma(ret);
        return ret;
    }

    std::string parseWorkspace(std::any data) {
        const auto&        workspace = std::any_cast<PHLWORKSPACE>(data);
        const std::string& ret       = CHyprCtl::getWorkspaceData(workspace, eHyprCtlOutputFormat::FORMAT_JSON);
        return ret;
    }

    std::string parseCWorkspace(std::any data) {
        const auto&        workspace = std::any_cast<CWorkspace*>(data);
        const std::string& ret       = parseWorkspace(workspace->m_self.lock());
        return ret;
    }

    std::string parseMonitor(std::any data) {
        const auto& monitor = std::any_cast<PHLMONITOR>(data);
        std::string ret     = CHyprCtl::getMonitorData(monitor->m_self.lock(), eHyprCtlOutputFormat::FORMAT_JSON);
        trimTrailingComma(ret);
        return ret;
    }

    std::string parseVectorWorkspaceMonitor(std::any data) {
        std::string ret = "[";
        const auto& vec = std::any_cast<std::vector<std::any>>(data);
        for (const auto& it : vec) {
            if (it.type() == typeid(PHLWORKSPACE)) {
                ret += parseWorkspace(it);
            } else if (it.type() == typeid(CMonitor)) {
                ret += CHyprCtl::getMonitorData(std::any_cast<CMonitor*>(it)->m_self.lock(), eHyprCtlOutputFormat::FORMAT_JSON);
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
            "{ \"timeMs\": \"" + std::to_string(event.timeMs) + "\", \"button\": \"" + std::to_string(event.button) + "\", \"state\": \"" + std::to_string(event.state) + "\" }";
        return ret;
    }

    std::string parseSDownEvent(std::any data) {
        const auto&        event = std::any_cast<ITouch::SDownEvent>(data);
        const std::string& ret =
            "{ \"timeMs\": \"" + std::to_string(event.timeMs) + "\", \"touchID\": \"" + std::to_string(event.touchID) + "\", \"pos\": \"" + parseVector2D(event.pos) + "\" }";
        return ret;
    }

    std::string parseSUpEvent(std::any data) {
        const auto&        event = std::any_cast<ITouch::SUpEvent>(data);
        const std::string& ret   = "{ \"timeMs\": \"" + std::to_string(event.timeMs) + "\", \"touchID:\" \"" + std::to_string(event.touchID) + "\" }";
        return ret;
    }
} // namespace Parser
