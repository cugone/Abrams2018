#include "Engine/UI/UISystem.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Thirdparty/Imgui/imgui.h"
#include "Thirdparty/Imgui/imgui_impl_dx11.h"
#include "Thirdparty/Imgui/imgui_impl_win32.h"

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

UISystem::UISystem(Renderer* renderer)
    : EngineSubsystem()
    , _renderer(renderer)
{
    IMGUI_CHECKVERSION();
    _context = ImGui::CreateContext();
    _io = &ImGui::GetIO();
}

UISystem::~UISystem() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext(_context);
    _context = nullptr;
    _io = nullptr;

    _renderer = nullptr;
}

void UISystem::Initialize() {
    auto hwnd = _renderer->GetOutput()->GetWindow()->GetWindowHandle();
    auto dx_device = _renderer->GetDevice()->GetDxDevice();
    auto dx_context = _renderer->GetDeviceContext()->GetDxContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(dx_device, dx_context);

    ImGui::StyleColorsDark();

}

void UISystem::BeginFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void UISystem::Update(TimeUtils::FPSeconds /*deltaSeconds*/) {
    /* DO NOTHING */
}

void UISystem::Render() const {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UISystem::EndFrame() {
    ImGui::EndFrame();
}

bool UISystem::ProcessSystemMessage(const EngineMessage& msg) {
    if(_io->WantCaptureKeyboard || _io->WantCaptureMouse) {
        return ImGui_ImplWin32_WndProcHandler(reinterpret_cast<HWND>(msg.hWnd), msg.nativeMessage, msg.wparam, msg.lparam);
    }
    return false;
}

