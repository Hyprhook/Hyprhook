
#define WLR_USE_UNSTABLE
#include "Parser.hpp"
#include <hyprland/src/plugins/PluginAPI.hpp>
#define private public
#include <hyprland/src/debug/HyprCtl.hpp>
#undef private

namespace Parser {

    std::string parseEmpty(std::any data) {
        const std::string& ret = "";
        return ret;
    }

    std::string parseSubmap(std::any data) {
        const auto&       submap = std::any_cast<std::string>(data);
        const std::string ret = "{ \"submap\": \"" + submap + "\" }";
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