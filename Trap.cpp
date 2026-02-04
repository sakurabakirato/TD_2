#include "Trap.h"
#include "UpDate.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "Player.h"
using namespace KamataEngine;

void Trap::Initialize(Model* model, Camera* camera,const Vector3& position)
{
	model_ = model;
	camera_ = camera;
	position_ = position;

	worldTransform_.Initialize();
	worldTransform_.translation_.y = 10;
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.5f, 0.5f, 1.0f };

	aabb_.min = { position.x - 0.5f, position.y - 0.5f, position.z - 0.5f };
	aabb_.max = { position.x + 0.5f, position.y + 0.5f, position.z + 0.5f };
}

void Trap::Update()
{
	if (isDead_) return;

	//Novice::ScreenPrintf(0, 100, "Trap Draw OK");
	// AABB更新 
	aabb_.min = { position_.x - 0.5f, position_.y - 0.5f, position_.z - 0.5f };
	aabb_.max = { position_.x + 0.5f, position_.y + 0.5f, position_.z + 0.5f };

	if (step == 1) 
	{
		isDead_ = true;
	}
}

void Trap::OnCollision(const Player* player)
{
	(void)player;
}

void Trap::Draw()
{
	if (isDead_) return;

	worldTransform_.translation_ = position_;

	worldTransform_.matWorld_ = MakeAffineMatrix(
		worldTransform_.scale_,
		worldTransform_.rotation_,
		worldTransform_.translation_
	);

	worldTransform_.TransferMatrix();

	model_->Draw(worldTransform_, *camera_);
}




