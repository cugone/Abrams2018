#pragma once

#include "Engine/Core/Config.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Profiling/Profiler.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"

extern Renderer* g_theRenderer;
extern Game* g_theGame;
extern InputSystem* g_theInput;
extern Console* g_theConsole;
extern Config* g_theConfig;
extern EngineSubsystem* g_theSubsystemHead;
extern App* g_theApp;
extern JobSystem* g_theJobSystem;
extern FileLogger* g_theFileLogger;
extern Profiler* g_theProfiler;