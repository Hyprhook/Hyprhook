#pragma once
#include <shared_mutex>
#include <string>
#include <any>

namespace Parser {
    // hyprctl usage lock
    std::string parseWindow(std::any data);
    std::string parseWorkspace(std::any data);
    std::string parseCWorkspace(std::any data);
    std::string parseMonitor(std::any data);
    std::string parseVectorWorkspaceMonitor(std::any data);
    std::string parseVetorWindowWorkspace(std::any data);
    std::string parseVector2D(std::any data);
    std::string parseSButtonEvent(std::any data);
    std::string parseSDownEvent(std::any data);
    std::string parseSUpEvent(std::any data);
    std::string parseEmpty(std::any data);
    std::string parseSubmap(std::any data);
}
