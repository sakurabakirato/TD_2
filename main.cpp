#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "GameClear.h"
#include "GameOver.h"
#include <Windows.h>

using namespace KamataEngine;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
GameClear* gameClear = nullptr;
GameOver* gameOver = nullptr;

// 02_12 25枚目(Scene sceneまで)
enum class Scene 
{
	kUnknown = 0,
	kTitle,
	kGame,
	kClear,
	kOver,
};

// 現在シーン（型）
Scene scene = Scene::kUnknown;

// 02_12 29枚目 
void ChangeScene() 
{

	switch (scene) 
	{
	case Scene::kTitle:
		if (titleScene->IsFinished()) 
		{
			// シーン変更
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();

			gameOver = new GameOver;
			gameOver->Initialize();

			gameClear = new GameClear;
			gameClear->Initialize();

		}
		break;

	case Scene::kGame:
		// 02_12 30枚目
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kClear;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;

	case Scene::kClear:
		if (gameScene->IsFinished()) {
			titleScene = new TitleScene;
			titleScene->Initialize();
			scene = Scene::kClear;
			delete gameClear;
			gameScene = nullptr;
		} else if (gameScene->IsFinished()) {
			scene = Scene::kOver;
			titleScene = new TitleScene;
			titleScene->Initialize();
			delete gameScene;
			gameScene = nullptr;
		}
		break;

	case Scene::kOver:
		// 02_12 30枚目
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

// 02_12 31枚目
void UpDataScene() {

	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kClear:
		break;
	case Scene::kOver:
		break;
	}
}

// 02_12 32枚目
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kClear:

		break;
	case Scene::kOver:

		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize();

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// gameScene->Initialize();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// シーン切り替え
		ChangeScene();

		// 現在シーン更新
		UpDataScene();

		// 描画更新
		dxCommon->PreDraw();

		// ゲームシーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	// 02_12 35枚目 各種解放
	delete titleScene;
	delete gameScene;

	// nullptrの代入
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}