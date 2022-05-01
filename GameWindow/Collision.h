/*==============================================================================

	ìñÇΩÇËîªíËèàóùÅ@[Collision.h]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#ifndef COLLISION_H_
#define COLLISION_H_

#include <d3dx9.h>

typedef struct CircleCollision_tag
{
	D3DXVECTOR2 center;
	float radius;
}CircleCollision;

bool CollisionCircleAndCircleHit(const CircleCollision* pA, const CircleCollision* pB);

#endif // COLLISION_H_