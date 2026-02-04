#include "SlotSystem.h"
#include <cassert>
#include <cstdlib>

using namespace KamataEngine;

void SlotSystem::Initialize(Model* model, Camera* camera, const Vector3& position)
{
	assert(model);

	model_ = model;
	camera_ = camera;
	basePosition_ = position;

	// 各リール初期化
	for (int i = 0; i < kReelCount; i++) 
	{
		reels_[i].symbolIndex = rand() % kSymbolCount;
		reels_[i].offsetY = 0.0f;
		reels_[i].isSpinning = false;

		worldTransforms_[i].Initialize();
		worldTransforms_[i].translation_ = basePosition_;
		//worldTransforms_[i].translation_ = position;
		//worldTransforms_[i].translation_.y += i * 1.2f; // 縦並び
	}
}

void SlotSystem::Update()
{
	spinTimer_ += 1.0f / 60.0f;

	for (int i = 0; i < kReelCount; i++)
	{
		Reel& reel = reels_[i];
		WorldTransform& wt = worldTransforms_[i];

		if (reel.isSpinning)
		{
			// 縦に流す
			reel.offsetY -= kSpinSpeed;

			// 絵1枚分進んだら次の絵柄
			if (reel.offsetY <= -kSymbolHeight)
			{
				reel.offsetY += kSymbolHeight;
				reel.symbolIndex = (reel.symbolIndex + 1) % kSymbolCount;
			}

			// ★ このリールだけ停止
			if (spinTimer_ >= reel.stopTime)
			{
				reel.isSpinning = false;
				reel.offsetY = 0.0f;
			}
		}

		// ★ 1面だけ表示（上下に流れるように見せる）
		wt.translation_.y = basePosition_.y + i * 1.2f + reel.offsetY;

		// Boss.cpp と同じ更新方法
		upDate->WorldTransformUpData(wt);
	}
}

void SlotSystem::Draw()
{
	for (int i = 0; i < kReelCount; i++) 
	{
		model_->Draw(worldTransforms_[i], *camera_);
	}
}

void SlotSystem::StartSpin(int reelIndex)
{
	if (reelIndex < 0 || reelIndex >= kReelCount) 
	{
		return;
	}
	reels_[reelIndex].isSpinning = true;
}

void SlotSystem::StopSpin(int reelIndex)
{
	if (reelIndex < 0 || reelIndex >= kReelCount) 
	{
		return;
	}

	reels_[reelIndex].isSpinning = false;
	reels_[reelIndex].offsetY = 0.0f;
}

bool SlotSystem::IsSpinning(int reelIndex) const
{
	if (reelIndex < 0 || reelIndex >= kReelCount) 
	{
		return false;
	}
	return reels_[reelIndex].isSpinning;
}

void SlotSystem::StartAll()
{
	spinTimer_ = 0.0f;

	for (int i = 0; i < kReelCount; i++)
	{
		reels_[i].isSpinning = true;
		reels_[i].offsetY = 0.0f;

		// ★ リールごとに停止時間をずらす
		reels_[i].stopTime = 1.5f + i * 0.5f;
	}
}

void SlotSystem::StopAll()
{
	for (int i = 0; i < kReelCount; i++)
	{
		reels_[i].isSpinning = false;
		reels_[i].offsetY = 0.0f;

		reels_[i].symbolIndex = rand() % kSymbolCount;
	}
	char buf[128];
	sprintf_s(buf, "SlotResult r0=%d r1=%d r2=%d\n",
		reels_[0].symbolIndex,
		reels_[1].symbolIndex,
		reels_[2].symbolIndex);
	OutputDebugStringA(buf);
}

bool SlotSystem::IsFinished() const
{
	for (int i = 0; i < kReelCount; i++)
	{
		if (reels_[i].isSpinning)
		{
			return false;
		}
	}
	return true;
}

int SlotSystem::GetResult(int reelIndex) const
{
	if (reelIndex < 0 || reelIndex >= kReelCount)
	{
		return -1;
	}
	return reels_[reelIndex].symbolIndex;
}
