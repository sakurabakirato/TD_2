#include "GameScene.h"
//#include<winddi.h>
#include <string>

using namespace KamataEngine;
using namespace std;

std::chrono::steady_clock::time_point lastTime;

GameScene::~GameScene()
{
	delete debugCamera_;
	delete model_;
	delete blockModel_;
	delete skydome_;
	delete modelPlayer_;
	delete modelBoss_;
	delete modelSlot_;
	delete trapModel_;
	delete player_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine)
		{
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete mapChipField_;
	for (int i = 0; i < kBossCount; i++)
	{
		delete bosses_[i];
	}
	//トラップ
	for (int i = 0; i < kTrapCount; i++)
	{
		delete trap_[i];
	}
	delete deathParticles_;
	delete deathParticle_model_;
	delete fade_;
	delete slot_;

}

void GameScene::Initialize()
{
	//モデル読み込み
	modelPlayer_ = Model::CreateFromOBJ("player", true);/*自機*/
	modelSkydome_ = Model::CreateFromOBJ("skyDome", true);/*天球*/
	modelBoss_ = Model::CreateFromOBJ("boss");/*ボスモデル*/
	blockModel_ = Model::CreateFromOBJ("block");/*ブロック*/
	fallingBlockModel_ = Model::CreateFromOBJ("block");/*落ちてくるブロック*/
	deathParticle_model_ = Model::CreateFromOBJ("deathParticle");/*デス時のパーティクル*/
	trapModel_ = Model::CreateFromOBJ("block");/*トラップ*/
	modelSlotA_ = Model::CreateFromOBJ("speed");
	modelSlotB_ = Model::CreateFromOBJ("power");
	modelSlotC_ = Model::CreateFromOBJ("none");

	// サウンドデータの読み込み
	/*BGMHandle = Audio::GetInstance()->LoadWave("sound/maou_game_village10.mp3");*/
	// サウンドデータの読み込み
	BGMHandle = Audio::GetInstance()->LoadWave("sound/1123.mp3");
	voiceHandle = Audio::GetInstance()->PlayWave(BGMHandle, true, 0.1f);
	/*voiceHandle = Audio::GetInstance()->PlayWave(BGMHandle, true, 0.2f);*/

	debugCamera_ = new DebugCamera(1280, 720);

	// マップチップフィールド生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks_csv/blocks.csv");

	// 自キャラの生成
	player_ = new Player();
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &camera_, playerPosition);

	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();
	camera_.farZ = 1000.0f;

	// 02_03天球
	// skydome生成
	skydome_ = new Skydome();
	// 初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	GenerateBlocks();

	CController_ = new CameraController(); // 生成
	CController_->Initialize(&camera_); // 初期化
	CController_->SetTarget(player_); // 追従対象セット
	CController_->Reset(); // リセット

	CameraController::Rect cameraArea = { 12.0f, 100 - 12.0f, 6.0f, 6.0f };
	CController_->SetMovableArea(cameraArea);

	player_->SetMapChipField(mapChipField_);

	//スロットクラス
	slot_ = new SlotSystem();
	// 右下あたりに固定表示したい位置
	Vector3 slotPosition{};
	slotPosition.x = 6.0f;
	slotPosition.y = 3.0f;
	slotPosition.z = -3.0f;

	// スロット用モデル配列
	Model* slotModels[3] = {
		modelSlotA_,
		modelSlotB_,
		modelSlotC_
	};

	// 初期化（Bossと同じ形式）
	slot_->Initialize(slotModels, &camera_, slotPosition);

	// 02_11_16枚目 仮の生成処理 後で消す
	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(deathParticle_model_, &camera_, playerPosition);

	// 02_12 4枚目 ゲームプレイフェーズから開始
	phase_ = Phase::kFadeIn;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	lastTime = std::chrono::steady_clock::now();

	for (int i = 0; i < kBossCount; i++) 
	{
		bosses_[i] = new Boss();
		Vector3 bossPosition =
			GetRandomBossPosition();
		bosses_[i]->Initialize(
			modelBoss_,
			&camera_,
			mapChipField_,
			bossPosition
		);
	}
	//トラップ
	for (int i = 0; i < 10; i++)
	{
		int x = rand() % 50;
		int z = rand() % (mapChipField_->GetNumBlockVirtical());

		trapPosition =
			mapChipField_->GetMapChipPositionByIndex(x, z);


		trap_[i] = new Trap();
		trap_[i]->Initialize(trapModel_, &camera_, trapPosition);
	}
}

//ランダム
Vector3 GameScene::GetRandomBossPosition()
{
	while (true)
	{
		uint32_t x = rand() % mapChipField_->GetNumBlockHorizontal();
		uint32_t y = rand() % (mapChipField_->GetNumBlockVirtical());

		MapChipType under = mapChipField_->GetMapChipTypeByIndex(x, y);
		MapChipType above = mapChipField_->GetMapChipTypeByIndex(x, y + 1);


		// ブロックの上が空白ならOK
		if (under == MapChipType::kBlock &&
			above == MapChipType::kBlank)
		{

			return mapChipField_->GetMapChipPositionByIndex(x, y + 1);
		}
	}
}


void GameScene::GenerateBlocks()
{
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する

	worldTransformBlocks_.resize(numBlockVirtical);

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i)
	{
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i)
	{
		for (uint32_t j = 0; j < numBlockHorizontal; ++j)
		{
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock)
			{
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

SlotEffect ConvertSlotResult(int center)
{
	switch (center)
	{
	case 0:
		return SlotEffect::SpeedUp;
	case 1:
		return SlotEffect::AttackDown;
	default:
		return SlotEffect::None;
	}
}

void GameScene::Update()
{
	/*--------------ゲームタイマー----------------*/
	//現在時間を取得
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaTime = now - lastTime;

	if (finishTime_)
	{
		return;
	}
	if (deltaTime.count() >= 1.0f)
	{
		GameTimer--;
		lastTime = now;
	}
	if (GameTimer <= 0)
	{
		finishTime_ = true;
		GameTimer = 0;
	}
	switch (phase_)
	{
	case Phase::kPlay:
		// ===== スロット自動回転 =====
		if (!isSlotSpinning_)
		{
			// ゲーム開始時に回し始める
			slot_->StartAll();
			isSlotSpinning_ = true;
			slotTimer_ = 0.0f;
		}
		// 回転中
		if (isSlotSpinning_)
		{
			slotTimer_ += 1.0f / 60.0f;

			if (slotTimer_ >= kSlotSpinTime_)
			{
				slot_->StopAll();
				isSlotSpinning_ = false;
			}
		}
		break;
	case Phase::kDeath:
		// デス演出フェーズ
		// 02_11 18枚目 デスパーティクルあれば更新
		deathParticles_->Update();

		if (deathParticles_ && deathParticles_->IsFinished())
		{
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished())
		{
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished())
		{
			finished_ = true;
		}
		break;
	case Phase::kGameClear:
		fade_->Update();
		if (fade_->IsFinished())
		{
			finished_ = true;
		}
		break;
	case Phase::kGameOver:
		fade_->Update();
		if (fade_->IsFinished())
		{
			finished_ = true;
			// Gameの音を止める
			Audio::GetInstance()->StopWave(voiceHandle);
		}
		break;
	}

	//skydome更新
	skydome_->Update();
	//自キャラの更新
	player_->Update();
	//敵更新
	for (int i = 0; i < kBossCount; i++)
	{
		if (bosses_[i] != nullptr)
		{
			bosses_[i]->Update();
		}
	}
	//スロット更新
	slot_->Update();

	if (slot_->IsFinished() && !slotResultApplied_)
	{
		OutputDebugStringA("Slot Finished!\n");

		slotResultApplied_ = true;

		int r = slot_->GetResult(rand() % 3);
		SlotEffect effect = ConvertSlotResult(r);
		player_->ApplySlotEffect(effect);

		// 🔽 次のスロットに備えてリセット
		slotResultApplied_ = false;
		isSlotSpinning_ = false;
		slotTimer_ = 0.0f;
	}

	//トラップ更新
	for (int i = 0; i < kTrapCount; i++)
	{
		if (trap_[i] != nullptr)
		{
			trap_[i]->Update();
		}
	}

	ChangePhase();
	CheckAllCollisions();

	// カメラの処理
	if (isDebugCameraActive_)
	{
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送AL3_02_02*/
		camera_.TransferMatrix();
	} 
	else
	{
		// ビュープロジェクション行列の更新と転送AL3_02_02*/
		camera_.UpdateMatrix();
	}
	CController_->Updata();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine)
		{
			if (!worldTransformBlock)
				continue;
			// アフィン変換行列の生成
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	//ボス攻撃チェック
	for (int i = 0; i < kBossCount; i++)
	{
		if (!bosses_[i]) continue;

		if (bosses_[i]->ShouldDropBlock())
		{
			// プレイヤーのX位置に落とす
			Vector3 playerPos = player_->GetWorldPosition();

			Vector3 spawnPos =
			{
				playerPos.x,
				playerPos.y + 20.0f,   // 高い位置から落とす
				playerPos.z
			};

			FallingBlock* block = new FallingBlock();
			block->Initialize(fallingBlockModel_, &camera_, spawnPos);
			fallingBlocks_.push_back(block);

			bosses_[i]->ConsumeBlockAttack();
		}
	}
	//落下ブロック更新
	for (auto it = fallingBlocks_.begin(); it != fallingBlocks_.end(); )
	{
		(*it)->Update();

		if ((*it)->IsDead())
		{
			delete* it;
			it = fallingBlocks_.erase(it);
		} 
		else
		{
			++it;
		}
	}
}

void GameScene::Draw()
{

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine)
		{
			if (!worldTransformBlock)
				continue;
			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}

	if (phase_ == Phase::kPlay || phase_ == Phase::kFadeIn)
	{
		player_->Draw();
	}
	if (phase_ == Phase::kGameOver)
	{
		gameOverSprite_->Draw();
	}

	// 天球描画
	skydome_->Draw();
	// 落下ブロック描画
	for (FallingBlock* block : fallingBlocks_)
	{
		block->Draw();
	}
	// スロット描画
	slot_->Draw();

	// 敵キャラ描画
	for (int i = 0; i < kBossCount; i++)
	{
		bosses_[i]->Draw();
	}
	// トラップ描画
	for (int i = 0; i < kTrapCount   ; i++)
	{
		trap_[i]->Draw();
	}

	// 02_11 18枚目 デスパーティクルあれば描画
	if (deathParticles_)
	{
		deathParticles_->Draw();
	}

	Model::PostDraw();

	fade_->Draw();
}

// 02_10 16枚目
void GameScene::CheckAllCollisions()
{
	// 判定対象1と2の座標
	AABB aabb1, aabb2;

#pragma region 自キャラと敵キャラの当たり判定
	{
		for (int i = 0; i < kBossCount; i++)
		{
			if (bosses_[i]->IsCollisionDisabled())
				continue;

			aabb1 = player_->GetAABB();
			aabb2 = bosses_[i]->GetAABB();

			if (IsCollision(aabb1, aabb2))
			{
				if (player_->IsAttack())
				{
					bosses_[i]->OnCollision(player_);
				} 
				else
				{
					player_->OnCollision(bosses_[i]);
				}
			}
		}
	}
#pragma endregion

#pragma region 自キャラと落下ブロックの当たり判定
	{
		// 自キャラのAABB
		aabb1 = player_->GetAABB();

		// 全ての落下ブロックと当たり判定
		for (FallingBlock* block : fallingBlocks_)
		{
			if (block->IsDead()) continue;

			// ブロックのAABB
			aabb2 = block->GetAABB();

			// 衝突判定
			if (IsCollision(aabb1, aabb2))
			{
				player_->OnCollision(block);
				block->OnCollision(player_);
			}
		}
	}
#pragma endregion

#pragma region 自キャラとトラップの当たり判定
	{
		// 自キャラのAABB
		aabb1 = player_->GetAABB();

		for (int i = 0; i < kTrapCount; i++)
		{
			// 全ての落下ブロックと当たり判定

			if (trap_[i]->IsDead()) continue;

				 //ブロックのAABB
			aabb2 = trap_[i]->GetAABB();

			// 衝突判定
			if (IsCollision(aabb1, aabb2))
			{
				player_->OnCollision(trap_[i]);
				trap_[i]->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

void GameScene::ChangePhase()
{
	switch (phase_)
	{
	case Phase::kPlay:
		if (player_->IsDead())
		{

			//音を止める
			Audio::GetInstance()->StopWave(voiceHandle);

			// 死亡演出
			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_->Initialize(deathParticle_model_, &camera_, deathParticlesPosition);

			// Gameの音を止める
			Audio::GetInstance()->StopWave(voiceHandle);
		}
		if (GameTimer == 0)
		{
			// Gameの音を止める
			Audio::GetInstance()->StopWave(voiceHandle);
			// 画面暗くする
			fade_->Start(Fade::Status::FadeIn, 1.0f);
			// 敵が死んでクリアに
			phase_ = Phase::kGameClear;
		}
		if (IsDead())
		{
			// Gameの音を止める
			Audio::GetInstance()->StopWave(voiceHandle);

			// Gameの音を止める
			Audio::GetInstance()->StopWave(voiceHandle);
			// 画面暗くする
			fade_->Start(Fade::Status::FadeIn, 1.0f);
			// 敵が死んでクリアに
			phase_ = Phase::kGameClear;
		}
		break;
	case Phase::kDeath:
		// デスパーティクルが終了したらゲームオーバー
		if (deathParticles_ && deathParticles_->IsFinished())
		{
			fade_->Start(Fade::Status::FadeIn, 1.0f);   // ゲームオーバー画面をフェードイン
		}
		break;
	case Phase::kGameClear:
		break;
	case Phase::kGameOver:
		break;
	}
}