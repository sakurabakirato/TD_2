#include "Boss.h"
#include "UpData.h"
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

void Boss::Initialize(Model* model, Camera* camera, MapChipField* mapChipField, const Vector3& position)
{

	assert(model);

	// 02_09 7枚目
	model_ = model;
	// 02_09 7枚目
	camera_ = camera;

	mapChipField_ = mapChipField;

	// 02_09 7枚目
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 02_09 7枚目 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> *3.0f / 2.0f;

	// 02_09 16枚目
	velocity_ = { -kWalkSpeed, 0, 0 };

	isFacingRight_ = false;

	// 02_09 20枚目
	walkTimer = 0.0f;


	// ---- AABB 初期化 ----
	const float halfSize = 0.5f; // ボスの大きさに応じて調整
	aabb_.min = { position.x - halfSize, position.y - halfSize, position.z - halfSize };
	aabb_.max = { position.x + halfSize, position.y + halfSize, position.z + halfSize };

}

// 02_09 スライド5枚目
void Boss::UpDate() 
{

	// 壁判定
	CheckWallAndReverse();

	// 02_09 16枚目 移動
	worldTransform_.translation_ += velocity_;

	// 02_09 20枚目
	walkTimer += 1.0f / 60.0f;

	// 02_09 23枚目 回転アニメーション
	// worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

	float param = std::sin(std::numbers::pi_v<float> *2.0f * walkTimer / kWalkMotionTime);

	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;

	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);

	// 02_09 スライド8枚目 ワールド行列更新
	upData->WorldTransformUpData(worldTransform_);

// ---------------------------
 // ★ AABB（当たり判定）更新 ★
 // ---------------------------
	Vector3 worldPos = GetWorldPosition();

	aabb_.min = 
	{
		worldPos.x - kWidth / 2.0f,
		worldPos.y - kHeight / 2.0f,
		worldPos.z - kWidth / 2.0f
	};
	aabb_.max = 
	{
		worldPos.x + kWidth / 2.0f,
		worldPos.y + kHeight / 2.0f,
		worldPos.z + kWidth / 2.0f
	};

	// ==== ブロック落下攻撃タイマー ====
	blockAttackTimer_++;
	if (blockAttackTimer_ > 180)  // 3秒に1回発射
	{
		canBlockAttack_ = true;
		blockAttackTimer_ = 0;
	}

}

// 02_09 スライド5枚目
void Boss::Draw() 
{
	// 02_09 スライド9枚目  モデル描画
	model_->Draw(worldTransform_, *camera_);
}

// 02_10 スライド14枚目
AABB Boss::GetAABB() 
{

	/*Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };

	return aabb;*/
	return aabb_;
}

// 02_10 スライド14枚目
Vector3 Boss::GetWorldPosition() 
{

	Vector3 worldPos{};

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

// 02_10 スライド20枚目
void Boss::OnCollision(const Player* player) { (void)player; }

void Boss::CheckWallAndReverse() {

	// 現在位置からAABBを取得
	AABB aabb = GetAABB();

	// ボスの「足元」のタイルを調べる
	Vector3 pos = GetWorldPosition();

	// タイル座標に変換
	MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(pos);

	// 左側のタイル
	if (velocity_.x < 0) {
		if (mapChipField_->GetMapChipTypeByIndex(index.xIndex - 1, index.yIndex) == MapChipType::kBlock) {
			velocity_.x = +kWalkSpeed;
			isFacingRight_ = true;
		}
	}

	// 右側のタイル
	if (velocity_.x > 0) {
		if (mapChipField_->GetMapChipTypeByIndex(index.xIndex + 1, index.yIndex) == MapChipType::kBlock) {
			velocity_.x = -kWalkSpeed;
			isFacingRight_ = false;
		}
	}

	// モデルの向きを方向に合わせる
	worldTransform_.rotation_.y = isFacingRight_
		? std::numbers::pi_v<float> / 2.0f
		: std::numbers::pi_v<float> *3.0f / 2.0f;
}
