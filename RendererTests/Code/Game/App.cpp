#include "Game/App.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/GameCommon.hpp"

App::App() {
    /* DO NOTHING */
}

App::~App() {
    /* DO NOTHING */
}

bool App::IsQuitting() const {
    return _isQuitting;
}

void App::SetIsQuitting(bool quit) {
    _isQuitting = quit;
}

void App::Initialize() {
    /* DO NOTHING */
}

void App::RunFrame() {
    using namespace std::chrono;

    BeginFrame();

    static float previousFrameTime = TimeUtils::GetCurrentTimeElapsed<duration<float>>();
    float currentFrameTime = TimeUtils::GetCurrentTimeElapsed<duration<float>>();
    float deltaSeconds = currentFrameTime - previousFrameTime;
    previousFrameTime = currentFrameTime;

    deltaSeconds = MathUtils::Clamp(deltaSeconds, 0.0f, 0.16f);

    Update(deltaSeconds);
    Render();
    EndFrame();
}

void App::BeginFrame() {
    /* DO NOTHING */
}

void App::Update(float /*deltaSeconds*/) {
    /* DO NOTHING */
}

void App::Render() const {
    /* DO NOTHING */
}

void App::EndFrame() {
    /* DO NOTHING */
}
