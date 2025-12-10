#include "GameOver.h"
#include "Math.h"
#include <numbers>

GameOver::~GameOver() {
	delete modelSpace_;
	delete modelOver_;
}

void GameOver::Initialize() {

	modelOver_ = Model::CreateFromOBJ("GameOver", true);
	/*modelSpace_ = Model::CreateFromOBJ("space");*/

	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skyDome", true);
	skydome_->Initialize(modelSkydome_, &camera_);


	// カメラ初期化
	camera_.Initialize();

	const float kPlayerOver = 2.0f;

	worldTransformOver_.Initialize();

	worldTransformOver_.scale_ = {kPlayerOver, kPlayerOver, kPlayerOver};

	const float kSpaceScale = 3.0f;

	worldTransformSpace_.Initialize();

	worldTransformSpace_.scale_ = {kSpaceScale, kSpaceScale, kSpaceScale};

	//worldTransformPlayer_.rotation_.y = 0.95f * std::numbers::pi_v<float>;

	//worldTransformPlayer_.translation_.x = -2.0f;

	//worldTransformPlayer_.translation_.y = -10.0f;


}

void GameOver::Update() {

	// 02_12 27枚目
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}

	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeOverMove);

	float angle = counter_ / kTimeOverMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformOver_.translation_.y = std::sin(angle) + 10.0f;

	camera_.TransferMatrix();

	upData->WorldTransformUpData(worldTransformOver_);

	// アフィン変換～DirectXに転送(タイトル座標)
	upData->WorldTransformUpData(worldTransformSpace_);
	//  skydome生成
	skydome_->Update();
}

void GameOver::Draw() {

	 DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得

	 ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	// 天球描画
	skydome_->Draw();

	modelOver_->Draw(worldTransformOver_, camera_);
	
	/*modelSpace_->Draw(worldTransformSpace_, camera_);*/

	Model::PostDraw();
}