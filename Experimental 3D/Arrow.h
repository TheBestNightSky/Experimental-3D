#pragma once
#include "DrawableBase.h"

class Arrow : public DrawableBase<Arrow> {
public:
	Arrow(Graphics& gfx, float x, float y, float rad, float r = 0.0f, float b = 0.0f, float g = 0.0f);
	void Update(float dt) noexcept override;
	void SetPos(float x, float y, float rad) noexcept;
	void SetColor(float r, float b, float g) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	float width;
	float height;
	float X;
	float Y;
	float Rad;
	float R;
	float G;
	float B;
};