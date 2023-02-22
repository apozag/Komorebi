#include "GUI/GUI.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"

#include "Core/Engine.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"

#include "Core/Reflection/TypeDescriptors.h"
#include "GUI/ImGuiTypeVisitor.h"

#include "Scene/Scene.h"

void Render(float /*dt*/) {

  Engine::GetDefaultRendertarget()->Bind();

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  {
    ImGui::Begin("Editor Window");

    ImGuiTypeVisitor visitor(Engine::m_activeScene);
    Engine::m_activeScene->GetReflection().Accept(&visitor);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  }

}

bool GUIAttachment::IsBlocking() {
  ImGuiIO& io = ImGui::GetIO();
  return io.WantCaptureKeyboard || io.WantCaptureMouse;
}

void GUIAttachment::Setup() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(m_hwnd);
  ImGui_ImplDX11_Init(m_device, m_context);

  Engine::AddPostRenderCallback(Render);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GUIAttachment::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}