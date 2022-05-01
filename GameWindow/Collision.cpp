/*==============================================================================

	�����蔻�菈���@[Collision.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 15
==============================================================================*/
#include "Collision.h"

bool CollisionCircleAndCircleHit(const CircleCollision* pA, const CircleCollision* pB)
{
	D3DXVECTOR2 vec = pA->center - pB->center;
	// D3DXVec2LengthSq�c�x�N�g���̒����̓���Ԃ�
	return D3DXVec2LengthSq(&vec) < (pA->radius + pB->radius) * (pA->radius + pB->radius);
}