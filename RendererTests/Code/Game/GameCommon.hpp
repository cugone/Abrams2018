#pragma once

#include "Engine/Core/Config.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"

#include <memory>

extern Renderer* g_theRenderer;
extern Game* g_theGame;
extern InputSystem* g_theInput;
extern Console* g_theConsole;
extern Config* g_theConfig;
extern EngineSubsystem* g_theSubsystemHead;
extern std::unique_ptr<App> g_theApp;
extern std::unique_ptr<JobSystem> g_theJobSystem;
extern std::unique_ptr<FileLogger> g_theFileLogger;