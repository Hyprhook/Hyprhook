#pragma once

#include "Parser.hpp"
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <string>

namespace Global {
    inline HANDLE                                                         PHANDLE     = nullptr;
    const std::string                                                     pluginName  = "Hyprhook";
    const std::string                                                     configPName = "hyprhook";
    std::unordered_map<std::string, Hyprlang::STRING const*>              eventMap;
    std::unordered_map<std::string, CSharedPointer<HOOK_CALLBACK_FN>>     hookMap;
    std::unordered_map<std::string, bool>                                 enabledMap;
    std::unordered_map<std::string, std::function<std::string(std::any)>> functionsMap = {
        {"activeWindow", Parser::parseWindow},
        {"keyboardFocus", Parser::parseEmpty}, //wlr_surface*
        {"moveWorkspace", Parser::parseVectorWorkspaceMonitor},
        {"focusedMon", Parser::parseMonitor},
        {"moveWindow", Parser::parseVetorWindowWorkspace},
        {"openLayer", Parser::parseEmpty},  //CLayerSurface*
        {"closeLayer", Parser::parseEmpty}, //CLayerSurface*
        {"openWindow", Parser::parseWindow},
        {"closeWindow", Parser::parseWindow},
        {"windowUpdateRules", Parser::parseWindow},
        {"urgent", Parser::parseWindow},
        {"minimize", Parser::parseEmpty}, //std::vectorstd::any{PHLWINDOW, int64_t}
        {"monitorAdded", Parser::parseMonitor},
        {"monitorRemoved", Parser::parseMonitor},
        {"createWorkspace", Parser::parseCWorkspace},
        {"destroyWorkspace", Parser::parseCWorkspace},
        {"fullscreen", Parser::parseWindow},
        {"changeFloatingMode", Parser::parseWindow},
        {"workspace", Parser::parseWorkspace},
        {"submap", Parser::parseSubmap},
        {"mouseMove", Parser::parseVector2D},
        {"mouseButton", Parser::parseSButtonEvent},
        {"mouseAxis", Parser::parseEmpty}, //M: event:IPointer::SAxisEvent
        {"touchDown", Parser::parseSDownEvent},
        {"touchUp", Parser::parseSUpEvent},
        {"touchMove", Parser::parseEmpty},    //ITouch::SMotionEvent
        {"activeLayout", Parser::parseEmpty}, //std::vectorstd::any{SP, std::string}
        {"preRender", Parser::parseMonitor},
        {"screencast", Parser::parseEmpty}, //std::vector<uint64_t>{state, framesInHalfSecond, owner}
        {"render", Parser::parseEmpty},     //eRenderStage
        {"windowtitle", Parser::parseWindow},
        {"configReloaded", Parser::parseEmpty},  // nullptr
        {"preConfigReload", Parser::parseEmpty}, // nullptr
        {"keyPress", Parser::parseEmpty},        //M: event:IKeyboard::SButtonEvent, keyboard:SP<IKeyboard>
        {"pin", Parser::parseWindow},
        {"swipeBegin", Parser::parseEmpty},  //IPointer::SSwipeBeginEvent
        {"swipeUpdate", Parser::parseEmpty}, //IPointer::SSwipeUpdateEvent
        {"swipeEnd", Parser::parseEmpty}     //IPointer::SSwipeEndEvent
    };
} // namespace global
