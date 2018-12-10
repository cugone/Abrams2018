#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

#include <condition_variable>
#include <memory>

class Config;
class Console;
class Game;
class InputSystem;
class JobSystem;
class Renderer;
class FileLogger;

class App : public EngineSubsystem {
public:
    App(std::unique_ptr<JobSystem> jobSystem, std::unique_ptr<FileLogger> fileLogger);
    virtual ~App();

    bool IsQuitting() const;
    void SetIsQuitting(bool quit);
    void Initialize() override;

    void RunFrame();
    bool HasFocus() const;
    bool LostFocus() const;
    bool GainedFocus() const;

    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

    static bool applet_mode;
protected:
private:
    virtual void BeginFrame() override;
    virtual void Update([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    bool _isQuitting = false;
    bool _current_focus = true;
    bool _previous_focus = false;
    std::unique_ptr<JobSystem> _theJobSystem{};
    std::unique_ptr<FileLogger> _theFileLogger{};
    std::unique_ptr<Config> _theConfig{};
    std::unique_ptr<Renderer> _theRenderer{};
    std::unique_ptr<InputSystem> _theInputSystem{};
    std::unique_ptr<Console> _theConsole{};
    std::unique_ptr<Game> _theGame{};
};