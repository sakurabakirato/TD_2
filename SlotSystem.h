#pragma once
#include "KamataEngine.h"
#include "Math.h"
#include "UpDate.h"

using namespace KamataEngine;

enum class SlotEffect
{
	None,        // 何も起きない
	SpeedUp,     // 移動速度UP
	AttackDown,  // 攻撃力DOWN
};

class SlotSystem
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model_"></param>
	/// <param name="camera_"></param>
	/// <param name="position"></param>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// リール回転開始
	/// </summary>
	void StartSpin(int reelIndex);

	/// <summary>
	/// リール停止
	/// </summary>
	void StopSpin(int reelIndex);

	/// <summary>
	/// 回転中か
	/// </summary>
	bool IsSpinning(int reelIndex) const;

	void StartAll();
	void StopAll();

	// 全リール停止したか？
	bool IsFinished() const;

	// リールの最終絵柄取得
	int GetResult(int reelIndex) const;

private:

	SlotEffect resultEffect_ = SlotEffect::None;

	// ===== 定数 =====
	static inline const int kReelCount = 3;     // リール数
	static inline const int kSymbolCount = 5;   // 絵柄数

	// ===== 内部構造 =====
	struct Reel
	{
		int symbolIndex = 0;   // 表示中の絵柄
		float offsetY = 0.0f; // 縦スクロール量
		bool isSpinning = false;

		float stopTime = 0.0f;   //このリールが止まる時間
	};

	// ===== リール =====
	Reel reels_[kReelCount];

	// ===== ワールド変換 =====
	WorldTransform worldTransforms_[kReelCount];

	// ===== 描画関連 =====
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	UpDate* upDate = nullptr;

	// ===== パラメータ =====
	static inline const float kSpinSpeed = 0.15f;   // 回転速度
	static inline const float kSymbolHeight = 1.0f; // 1マス分の高さ

	Vector3 basePosition_;

	float spinTimer_ = 0.0f;   // 全体の経過時間

};