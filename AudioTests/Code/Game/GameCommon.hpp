#pragma once

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/App.hpp"
#include "Game/Game.hpp"

extern App* g_theApp;
extern Renderer* g_theRenderer;
extern Game* g_theGame;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern Console* g_theConsole;
extern EngineSubsystem* g_theSubsystemHead;
extern FileLogger* g_theFileLogger;