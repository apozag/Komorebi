#include "GUI/GUI.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"

#include "ImGuizmo/ImGuizmo.h"

#include "Core/Engine.h"
#include "Core/Window.h"
#include "Core/Memory/Factory.h"
#include "Core/Reflection/UnloadTypeVisitor.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"

#include "Core/Reflection/TypeDescriptors.h"
#include "GUI/ImGuiTypeVisitor.h"

#include "GUI/SceneGUIWindow.h"
#include "GUI/RendererGUIWindow.h"
#include "GUI/PrefabGUIWindow.h"
#include "GUI/MaterialGUIWindow.h"

#include "Scene/Scene.h"
#include "Scene/SceneLoader.h"

void DrawRawEditor(){
  ImGui::Begin("Raw Editor");

  ImGuiTypeVisitor visitor(Engine::GetActiveScene());
  reflection::TypeResolver<Scene>::get()->Accept(&visitor);

  ImGui::End();
}

void DrawSaveSceneMenu() {
  static bool setup = false;
  static constexpr size_t buffSize = 64;
  static char buff[buffSize];
  if (!setup) {
    buff[0] = '\0';
    setup = true;
  }
  ImGui::InputText("##SaveFileName", buff, buffSize);
  ImGui::SameLine();
  if (ImGui::Button("Save##SaveButton") && buff[0] != '\0') {
    SceneLoader::SaveScene(Engine::GetActiveScene(), buff);
  }
  ImGui::EndMenu();
}

void DrawLoadSceneMenu() {
  static bool setup = false;
  static constexpr size_t buffSize = 64;
  static char buff[buffSize];
  if (!setup) {
    buff[0] = '\0';
    setup = true;
  }
  ImGui::InputText("##LoadFileName", buff, buffSize);
  ImGui::SameLine();
  if (ImGui::Button("Load##LoadButton") && buff[0] != '\0') {
    //memory::Factory::FreeAll();
    reflection::UnloadTypeVisitor unloadVisitor (Engine::GetActiveScene()->GetRootNode());
    reflection::TypeResolver<Node>::get()->Accept(&unloadVisitor);
    Engine::SetActiveScene(SceneLoader::LoadScene(buff));
  }
  ImGui::EndMenu();
}

void DrawTopMenu() {
  ImGui::BeginMainMenuBar();

  // File Menu
  if (ImGui::BeginMenu("File"))
  {

    // Save Scene
    if (ImGui::BeginMenu("Save Scene"))
    {
      DrawSaveSceneMenu();
    }
    // Load Scene
    if (ImGui::BeginMenu("Load Scene"))
    {
      DrawLoadSceneMenu();
    }

    ImGui::EndMenu();
  }

  // Raw Editor
  {
    static bool enabled = false;
    if (ImGui::Button("Raw Editor")) enabled = !enabled;
    if (enabled) DrawRawEditor();
  }

  // Scene
  {
    static bool enabled = false;
    if (ImGui::Button("Scene")) enabled = !enabled;
    if (enabled) DrawSceneGUIWindow();
  }

  // Renderer
  {
    static bool enabled = false;
    if (ImGui::Button("Renderer")) enabled = !enabled;
    if (enabled) DrawRendererGUIWindow();
  }

  // Prefabs
  {
    static bool enabled = false;
    if (ImGui::Button("Prefabs")) enabled = !enabled;
    if (enabled) DrawPrefabGUIWindow(); 
  }

  // Materials
  {
    static bool enabled = false;
    if (ImGui::Button("Materials")) enabled = !enabled;
    if (enabled) DrawMaterialGUIWindow();
  }

  ImGui::EndMainMenuBar();
}

void Render(float /*dt*/) {

  Engine::GetDefaultRendertarget()->Bind();

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  ImGuizmo::BeginFrame();

  {
    DrawTopMenu();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  }

}

bool GUIAttachment::IsBlocking() {
  ImGuiIO& io = ImGui::GetIO();
  return io.WantCaptureKeyboard || io.WantCaptureMouse || ImGuizmo::IsUsing();
}

void GUIAttachment::Setup() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(m_hwnd);
  ImGui_ImplDX11_Init(m_device, m_context);

  Engine::AddPostRenderCallback(Render);

  SetupSceneGUIWindow();
  SetupRendererGUIWindow();
  SetupPrefabGUIWindow();
  SetupMaterialGUIWindow();

  ImGui::GetIO().DisplaySize.x = Engine::GetWindow()->GetClientWidth();
  ImGui::GetIO().DisplaySize.y = Engine::GetWindow()->GetClientHeight();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GUIAttachment::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}