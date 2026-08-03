#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "UpDate.h"

using namespace KamataEngine;

class Player;

class FallingBlock
{
public:
    void Initialize(Model* model, Camera* camera, const Vector3& pos);
    void Update();
    void Draw();

    AABB GetAABB() const { return aabb_; }
    bool IsDead() const { return isDead_; }

    void OnCollision(const Player* player);

private:
    WorldTransform worldTransform_;
    Model* model_ = nullptr;
    Camera* camera_ = nullptr;

    Vector3 position_ = {};
    Vector3 velocity_ = {};
    AABB aabb_ = {};
    bool isDead_ = false;
};

