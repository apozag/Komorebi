#pragma once

#include "StateBindable.h"

class RasterizerState : public StateBindable{
public:
	RasterizerState(bool cullFront = false, bool wireframe = false) : m_cullFront(cullFront), m_wireframe(wireframe) {}
	~RasterizerState();

	void Setup() override;

	void Bind()const override;
	void Unbind()const override;

	REFLECT()

private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer;

	//////////////////
	// Serializable
	//////////////////

	bool m_cullFront;
	bool m_wireframe;
};