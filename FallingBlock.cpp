#include "FallingBlock.h"
#include "MapChipField.h"
#include "Math.h"
#include "UpDate.h"
#include <algorithm>
#include <cassert>
#include <numbers>


void FallingBlock::Initialize(Model* model, Camera* camera, const Vector3& pos)
{
    model_ = model;
    camera_ = camera;

    position_ = pos;
    velocity_ = { 0, -0.3f, 0 };   // 落下速度

    worldTransform_.Initialize();

    aabb_.min = { pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f };
    aabb_.max = { pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f };
}

void FallingBlock::Update()
{
    if (isDead_) return;

    // 落下
    position_ += velocity_;

    // AABB更新 
    aabb_.min = { position_.x - 0.5f, position_.y - 0.5f, position_.z - 0.5f };
    aabb_.max = { position_.x + 0.5f, position_.y + 0.5f, position_.z + 0.5f };

    // 地面に当たったら消滅
    if (position_.y < 0.0f)
    {
        isDead_ = true;
    }
}

void FallingBlock::OnCollision(const Player* player) 
{ 
    (void)player; 
}

void FallingBlock::Draw()
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

