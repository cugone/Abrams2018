#include "Game/GameCommon.hpp"

Renderer* g_theRenderer = nullptr;
Game* g_theGame = nullptr;
InputSystem* g_theInput = nullptr;
Console* g_theConsole = nullptr;
Config* g_theConfig = nullptr;
EngineSubsystem* g_theSubsystemHead = nullptr;
std::unique_ptr<App> g_theApp{};
std::unique_ptr<JobSystem> g_theJobSystem{};
std::unique_ptr<FileLogger> g_theFileLogger{};