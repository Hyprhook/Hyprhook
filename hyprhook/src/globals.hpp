#pragma once

#include <hyprland/src/plugins/PluginAPI.hpp>

inline HANDLE                                                                PHANDLE = nullptr;

static std::vector<std::string>                                              events;
static std::unordered_map<std::string, Hyprlang::STRING const*>              eventMap;
static std::unordered_map<std::string, CSharedPointer<HOOK_CALLBACK_FN>>     hookMap;
static std::unordered_map<std::string, bool>                                 enabledMap;

static std::string                                                           parseWindow(std::any data); //tested
static std::string                                                           parseWorkspace(std::any data);
static std::string                                                           parceCWorkspace(std::any data);
static std::string                                                           parseMonitor(std::any data);
static std::string                                                           parseVectorWorkspaceMonitor(std::any data);
static std::string                                                           parseVetorWindowWorkspace(std::any data); //tested
static std::string                                                           parseVector2D(std::any data);
static std::string                                                           parseSButtonEvent(std::any data);

static std::unordered_map<std::string, std::function<std::string(std::any)>> functionsMap = {
    {"activeWindow", parseWindow},
    {"keyboardFocus", [](std::any data) { return ""; }}, //wlr_surface*
    {"moveWorkspace", parseVectorWorkspaceMonitor},
    {"focusedMon", parseMonitor},
    {"moveWindow", parseVetorWindowWorkspace},
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
    {"mouseMove", parseVector2D},
    {"mouseButton", parseSButtonEvent},
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
