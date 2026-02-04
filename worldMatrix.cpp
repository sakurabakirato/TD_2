#include "worldMatrix.h"

void WorldTransformUpDate(KamataEngine::WorldTransform& worldTransform) 
{
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);

	worldTransform.TransferMatrix();
}


void WorldTransform::UpdateMatrix()
{
	Matrix4x4 matScale = MakeScaleMatrix(scale_);
	Matrix4x4 matRotX = MakeRotateXMatrix(rotation_.x);
	Matrix4x4 matRotY = MakeRotateYMatrix(rotation_.y);
	Matrix4x4 matRotZ = MakeRotateZMatrix(rotation_.z);
	Matrix4x4 matTrans = MakeTranslateMatrix(translation_);

	matWorld_ = matScale * matRotX * matRotY * matRotZ * matTrans;
}