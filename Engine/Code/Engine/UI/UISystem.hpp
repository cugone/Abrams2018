#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

class Renderer;
struct ImGuiContext;
struct ImGuiIO;

class UISystem : public EngineSubsystem {
public:
    UISystem(Renderer* renderer);
    UISystem(const UISystem& other) = default;
    UISystem(UISystem&& other) = default;
    UISystem& operator=(const UISystem& other) = default;
    UISystem& operator=(UISystem&& other) = default;
    virtual ~UISystem();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(TimeUtils::FPSeconds deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

protected:
private:
    Renderer* _renderer{};
    ImGuiContext* _context{};
    ImGuiIO* _io{};
};
