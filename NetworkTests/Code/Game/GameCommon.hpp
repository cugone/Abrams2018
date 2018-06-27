#pragma once

#include "Engine/Core/Console.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"

extern App* g_theApp;
extern Renderer* g_theRenderer;
extern Game* g_theGame;
extern InputSystem* g_theInput;
extern Console* g_theConsole;
extern EngineSubsystem* g_theSubsystemHead;
extern FileLogger* g_theFileLogger;
extern JobSystem* g_theJobSystem;