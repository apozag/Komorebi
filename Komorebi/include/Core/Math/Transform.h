#pragma once

#include "Core/Math/MathWrappers.h"
#include "Core/GameObject.h"

class Transform : public GameObject{
	friend class Scene;
public:
	Transform() {
		m_matrix = DirectX::XMMatrixIdentity();
		decompose();
	}
	Transform(DirectX::XMMATRIX matrix) {
		m_matrix = matrix;
		decompose();
	}

	void Setup() override;

	const DirectX::XMMATRIX&  GetMatrix() const {
		return m_matrix;
	}

	const DirectX::XMMATRIX& GetInverseMatrix() {
		if (m_invMatDirty) {
			m_invMatrix = DirectX::XMMatrixInverse(nullptr, m_matrix);
			m_invMatDirty = false;
		}
		return m_invMatrix;
	}

	const DirectX::XMMATRIX& GetInverseMatrixUnsafe() const {
		return m_invMatrix;
	}

	const DirectX::SimpleMath::Vector3& GetPosition(){
		if (m_decomposeDirty) decompose();
		return m_position;
	}
	const DirectX::SimpleMath::Vector3& GetScale() {
		if (m_decomposeDirty) decompose();
		return m_scale;
	}
	const DirectX::SimpleMath::Quaternion& GetRotation() {
		if (m_decomposeDirty) decompose();
		return m_rotation;
	}

	const DirectX::SimpleMath::Vector3&  GetPositionUnsafe() const {
		return m_position;
	}
	const DirectX::SimpleMath::Vector3& GetScaleUnsafe() const{
		return m_scale;
	}
	const DirectX::SimpleMath::Quaternion& GetRotationUnsafe() const{
		return m_rotation;
	}

	DirectX::SimpleMath::Vector3 GetForward() const{
		return DirectX::XMVector4Transform(DirectX::XMVECTORF32({ 0.f, 0.f, -1.f, 0.f }), m_matrix);
	}
	DirectX::SimpleMath::Vector3 GetRight() const {
		return DirectX::XMVector4Transform(DirectX::XMVECTORF32({ 1.f, 0.f, 0.f, 0.f }), m_matrix);
	}
	DirectX::SimpleMath::Vector3 GetUp() const {
		return DirectX::XMVector4Transform(DirectX::XMVECTORF32({ 0.f, 1.f, 0.f, 0.f }), m_matrix);
	}

	void SetPosition(DirectX::SimpleMath::Vector3 pos) {
		update();
		m_position = pos;
		compose();
		m_dirty = true;
	}
	void SetRotation(DirectX::SimpleMath::Quaternion rot) {
		update();
		m_rotation = rot;
		compose();
		m_dirty = true;
	}
	void SetScale(DirectX::SimpleMath::Vector3 scale) {
		update();
		m_scale = scale;
		compose();
		m_dirty = true;
	}
	void SetMatrix(DirectX::XMMATRIX matrix) {
		m_matrix = matrix;
		m_invMatDirty = true;
		m_dirty = true;
		decompose();
	}

	DirectX::SimpleMath::Vector3 PointToLocal(DirectX::SimpleMath::Vector3 point) {
		return DirectX::XMVector4Transform({point.x, point.y, point.z, 1.0f}, GetInverseMatrix());
	}

	DirectX::SimpleMath::Vector3 PointToLocalUnsafe(DirectX::SimpleMath::Vector3 point) const {
		return DirectX::XMVector4Transform({ point.x, point.y, point.z, 1.0f }, m_invMatrix);
	}

	DirectX::SimpleMath::Vector3 DirectionToWorld(DirectX::SimpleMath::Vector3 dir) const{
		return DirectX::XMVector4Transform({ dir.x, dir.y, dir.z, 0.0f }, GetMatrix());
	}

	DirectX::SimpleMath::Vector3 PointToWorld(DirectX::SimpleMath::Vector3 point) const{
		return DirectX::XMVector4Transform({ point.x, point.y, point.z, 1.0f }, GetMatrix());
	}

	DirectX::SimpleMath::Vector3 DirectionToLocal(DirectX::SimpleMath::Vector3 dir) {
		return DirectX::XMVector4Transform({ dir.x, dir.y, dir.z, 0.0f }, GetInverseMatrix());
	}

	DirectX::SimpleMath::Vector3 DirectionToLocalUnsafe(DirectX::SimpleMath::Vector3 dir) const {
		return DirectX::XMVector4Transform({ dir.x, dir.y, dir.z, 0.0f }, m_matrix);
	}

	void Translate(DirectX::SimpleMath::Vector3 translation) {
		m_matrix = m_matrix * DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
		m_decomposeDirty = true;
		m_dirty = true;
	}

	void TranslateLocal(DirectX::SimpleMath::Vector3 translation) {
		m_matrix =  DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) * m_matrix;
		m_decomposeDirty = true;
		m_dirty = true;
	}

	void RotateEulerLocal(DirectX::SimpleMath::Vector3 rotation) {
		m_matrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation) * m_matrix;
		m_decomposeDirty = true;
		m_invMatDirty = true;
		m_dirty = true; 
	}

	void RotateEuler(DirectX::SimpleMath::Vector3 rotation) {
		m_matrix = m_matrix * DirectX::XMMatrixRotationRollPitchYawFromVector(rotation) ;
		m_decomposeDirty = true;
		m_invMatDirty = true;
		m_dirty = true;
	}

	void RotateQuaternion(DirectX::SimpleMath::Quaternion rotation) {
		m_matrix = m_matrix * DirectX::XMMatrixRotationQuaternion(rotation);
		m_decomposeDirty = true;
		m_invMatDirty = true;
		m_dirty = true;
	}

	void RotateQuaternionLocal(DirectX::SimpleMath::Vector3 quaternion) {
		m_matrix = DirectX::XMMatrixRotationQuaternion(quaternion) * m_matrix;
		m_decomposeDirty = true;
		m_invMatDirty = true;
		m_dirty = true;
	}

	REFLECT_BASE()

private:

	void update() {
		if (m_decomposeDirty) {
			decompose();
			m_decomposeDirty = false;
		}
		if (m_invMatDirty) {
			m_invMatrix = DirectX::XMMatrixInverse(nullptr, m_matrix);
			m_invMatDirty = false;
		}
	}

	void decompose() {
		DirectX::XMVECTOR s, r, p;
		DirectX::XMMatrixDecompose(&s, &r, &p, m_matrix);
		m_position = p;
		m_rotation = r;
		m_scale = s;
	}

	void compose() {
		m_matrix = DirectX::XMMatrixTranslationFromVector(m_position)* DirectX::XMMatrixRotationQuaternion(m_rotation)* DirectX::XMMatrixScalingFromVector(m_scale);
		m_invMatDirty = true;
	}

	math::Vector3	m_position;
	math::Quaternion m_rotation;
	math::Vector3	m_scale;

	math::Matrix4 m_matrix;
	math::Matrix4 m_invMatrix;

	bool m_decomposeDirty = true;
	bool m_invMatDirty = true;
	bool m_dirty = true;
};

Transform operator*(Transform leftTransform, Transform rightTransform);
