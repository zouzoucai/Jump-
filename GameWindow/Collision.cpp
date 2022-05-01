/*==============================================================================

	当たり判定処理　[Collision.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 15
==============================================================================*/
#include "Collision.h"

bool CollisionCircleAndCircleHit(const CircleCollision* pA, const CircleCollision* pB)
{
	D3DXVECTOR2 vec = pA->center - pB->center;
	// D3DXVec2LengthSq…ベクトルの長さの二乗を返す
	return D3DXVec2LengthSq(&vec) < (pA->radius + pB->radius) * (pA->radius + pB->radius);
}