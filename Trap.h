#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "UpDate.h"

using namespace KamataEngine;

class Player;

class Trap
{
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

    AABB GetAABB() const { return aabb_; }
    bool IsDead() const { return isDead_; }

    void OnCollision(const Player* player);
  
    // 02_10 14枚目 当たり判定サイズ
    static inline const float kWidth = 0.8f;
    static inline const float kHeight = 0.8f;
private:
    WorldTransform worldTransform_;
    Model* model_ = nullptr;
    Camera* camera_ = nullptr;

    Vector3 position_ = {};
  
    AABB aabb_ = {};


    bool isDead_ = false;
    //踏んだのフラグ
    bool step = false;

};

