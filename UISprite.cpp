#include "UISprite.h"



void UISprite::Initialize(uint32_t textureHandle)
{
	//テクスチャ保存
	textureHandle_ = textureHandle;

	//WorldTransform 初期化
	worldTransform_.Initialize();

	//初期値を反映
	worldTransform_.translation_.x = position_.x;
	worldTransform_.translation_.y = position_.y;
	worldTransform_.translation_.z = 0.0f;

	worldTransform_.scale_.x = size_.x;
	worldTransform_.scale_.y = size_.y;
	worldTransform_.scale_.z = 1.0f;

	//行列更新
	worldTransform_.UpdateMatrix();
}

void UISprite::Update()
{

}

void UISprite::Draw()
{

}

void UISprite::SetPosition(float x, float y)
{
	position_.x = x;
	position_.y = y;
}

void UISprite::SetSize(float w, float h)
{
	size_.x = w;
	size_.y = h;
}
