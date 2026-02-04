#pragma once
#include "KamataEngine.h"
#include "Math.h"

class UISprite
{
public:
	void Initialize(uint32_t textureHandle);
	void Update();
	void Draw();

	void SetPosition(float x, float y);
	void SetSize(float w, float h);
private:

	Vector2 position_{};
	Vector2 size_{};

	uint32_t textureHandle_ = 0;

	WorldTransform worldTransform_;

};

