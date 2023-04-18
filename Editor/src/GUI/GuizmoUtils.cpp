#define _XM_NO_INTRINSICS_

#include <iostream>

#include "GUI\GizmoUtils.h"

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

#include "Core/Math/Transform.h"
#include "Core/Engine.h"
#include "Core/Window.h"
#include "Scene/Node.h"
#include "Entities/Camera.h"

void DrawImGuiTransform(Transform& localTransform, Transform& parentTransform) {
  float matrixTranslation[3], matrixRotation[3], matrixScale[3];
  DirectX::XMMATRIX xmMatrix = localTransform.GetMatrix();
  float transformMatrix[16] = {
     xmMatrix._11, xmMatrix._12, xmMatrix._13, xmMatrix._14,
     xmMatrix._21, xmMatrix._22, xmMatrix._23, xmMatrix._24,
     xmMatrix._31, xmMatrix._32, xmMatrix._33, xmMatrix._34,
     xmMatrix._41, xmMatrix._42, xmMatrix._43, xmMatrix._44
  };
  ImGuizmo::DecomposeMatrixToComponents(transformMatrix, matrixTranslation, matrixRotation, matrixScale);
  bool edited = false;
  edited |= ImGui::InputFloat3("Tr", matrixTranslation);
  edited |= ImGui::InputFloat3("Rt", matrixRotation);
  edited |= ImGui::InputFloat3("Sc", matrixScale);
  if (edited) {
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, transformMatrix);
    localTransform.SetMatrix({
      transformMatrix[0], transformMatrix[1], transformMatrix[2], transformMatrix[3],
      transformMatrix[4], transformMatrix[5], transformMatrix[6], transformMatrix[7],
      transformMatrix[8], transformMatrix[9], transformMatrix[10], transformMatrix[11],
      transformMatrix[12], transformMatrix[13], transformMatrix[14], transformMatrix[15],
      });
  }

  Camera* mainCamera = Engine::GetMainCamera();
  const DirectX::XMMATRIX xmProjMat = mainCamera->m_proj;
  float projMatrix[16] = {
     xmProjMat._11, xmProjMat._12, xmProjMat._13, xmProjMat._14,
     xmProjMat._21, xmProjMat._22, xmProjMat._23, xmProjMat._24,
     xmProjMat._31, xmProjMat._32, xmProjMat._33, xmProjMat._34,
     xmProjMat._41, xmProjMat._42, xmProjMat._43, xmProjMat._44
  };
  const DirectX::XMMATRIX xmViewMat =  mainCamera->GetNode()->m_localTransform.GetInverseMatrix();
  float viewMatrix[16] = {
     xmViewMat._11, xmViewMat._12, xmViewMat._13, xmViewMat._14,
     xmViewMat._21, xmViewMat._22, xmViewMat._23, xmViewMat._24,
     xmViewMat._31, xmViewMat._32, xmViewMat._33, xmViewMat._34,
     xmViewMat._41, xmViewMat._42, xmViewMat._43, xmViewMat._44
  };
  DirectX::XMMATRIX xmGlobalMat = (localTransform * parentTransform).GetMatrix();
  float globalMatrix[16] = {
     xmGlobalMat._11, xmGlobalMat._12, xmGlobalMat._13, xmGlobalMat._14,
     xmGlobalMat._21, xmGlobalMat._22, xmGlobalMat._23, xmGlobalMat._24,
     xmGlobalMat._31, xmGlobalMat._32, xmGlobalMat._33, xmGlobalMat._34,
     xmGlobalMat._41, xmGlobalMat._42, xmGlobalMat._43, xmGlobalMat._44
  };
  //float globalMatrixPrev[16];
  //memcpy(globalMatrixPrev, globalMatrix, 16 * sizeof(float));
  ImGuizmo::SetRect(0, 0, Engine::GetWindow()->GetClientWidth(), Engine::GetWindow()->GetClientHeight());
  if (ImGuizmo::Manipulate(viewMatrix, projMatrix, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, globalMatrix)) {

    /*std::ostringstream os_;

    os_ << "----------------------------------------------------------------\n";
    os_ << globalMatrix[12] << ", " << globalMatrix[13] << ", " << globalMatrix[14] << "\n";
    os_ << globalMatrixPrev[12] << ", " << globalMatrixPrev[13] << ", " << globalMatrixPrev[14] << "\n";

    OutputDebugString(os_.str().c_str());*/

    xmGlobalMat = {
      globalMatrix[0], globalMatrix[1], globalMatrix[2], globalMatrix[3],
      globalMatrix[4], globalMatrix[5], globalMatrix[6], globalMatrix[7],
      globalMatrix[8], globalMatrix[9], globalMatrix[10], globalMatrix[11],
      globalMatrix[12], globalMatrix[13], globalMatrix[14], globalMatrix[15],
    };
    localTransform.SetMatrix(DirectX::XMMatrixMultiply(xmGlobalMat, parentTransform.GetInverseMatrix()));
  }
}