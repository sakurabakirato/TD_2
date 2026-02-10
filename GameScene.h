#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "Fade.h"
#include "Boss.h"
#include "FallingBlock.h"
#include "SlotSystem.h"
#include "Trap.h"
#include <KamataEngine.h>

using namespace KamataEngine;

// ゲームシーン
class GameScene
{
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	~GameScene();

	void GenerateBlocks();
	// 02_10 16枚目 衝突判定と応答
	void CheckAllCollisions();
	// 02_12 9枚目
	void ChangePhase();
	// 02_12 26枚目	デスフラグのgetter
	bool IsFinished() const { return finished_; }
	//時間で生きる
	bool IsFinishTime()const { return finishTime_; };
	//bool IsGameClear() const { return isGameClear_; }
	bool IsDead() const { return isDead; }
	bool isDead = false;

	//ランダム
	Vector3 GetRandomBossPosition();

private:
	enum class Phase
	{
		kFadeIn,
		kPlay, // ゲームプレイ
		kDeath, // デス演出
		kFadeOut,
		kGameClear,
		kGameOver
	};

	// 02_12 4枚目 ゲームの現在フェーズ（変数）
	Phase phase_{};

	////テクスチャーハンドル
	uint32_t textureHandle_ = 0;

	Sprite* sprite_ = nullptr;
	UpDate* upDate = nullptr;

	WorldTransform worldTransform_;

	//カメラ
	Camera camera_;

	DebugCamera* debugCamera_ = nullptr;

	//オブジェクトの個数
	static const int kBossCount = 10;
	static const int kTrapCount = 10;
	//自キャラ
	Player* player_ = nullptr;
	//敵
	Enemy* enemy_ = nullptr;
	//ボス
	/*Boss* boss_ = nullptr;*/
	//スロット
	SlotSystem* slot_ = nullptr;
	//天球
	Skydome* skydome_ = nullptr;
	//ボス
	Boss* bosses_[kBossCount];
	//トラップ
	Trap* trap_[kTrapCount];

	MapChipField* mapChipField_;
	CameraController* CController_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;
	Fade* fade_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	//-----------------------3Dモデル--------------------------//
	Model* model_ = nullptr;
	Model* blockModel_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* enemy_model_ = nullptr;
	Model* modelBoss_ = nullptr;
	Model* modelSlot_ = nullptr;
	Model* deathParticle_model_ = nullptr;
	Model* fallingBlockModel_ = nullptr;
	Model* trapModel_ = nullptr;
	//--------------------------------------------------------//
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	std::list<Enemy*> enemies_;
	std::vector<FallingBlock*> fallingBlocks_;
	std::vector<Trap*> traps_;

	// 02_12 26枚目
	bool finished_ = false;

	Sprite* gameClearSprite_ = nullptr;
	Sprite* gameOverSprite_ = nullptr;

	bool isGameClear_ = false; // ボス撃破

	//BGM
	uint32_t BGMHandle = 0;
	uint32_t voiceHandle = 0;

	float GameTimer = 60;
	//時間でクリア
	bool finishTime_ = false;

	Input* input = Input::GetInstance();

	// ===== スロット制御 =====
	float slotTimer_ = 0.0f;
	bool isSlotSpinning_ = false;
	static constexpr float kSlotSpinTime_ = 2.0f; // 2秒回す

	bool slotResultApplied_ = false;

	Vector3 trapPosition;

	Model* modelSlotA_;
	Model* modelSlotB_;
	Model* modelSlotC_;


};