/*==============================================================================

	デバッグ用図形表示処理　[DebugPrimitive.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#ifndef DEBUGPRIMITIVE_H_
#define DEBUGPRIMITIVE_H_

void DebugPrimitive_Initialize(void);
void DebugPrimitive_Finalize(void);
void DebugPrimitive_BatchBegin(void);
void DebugPrimitive_BatchDrawCircle(float x, float y, float radius);
void DebugPrimitive_BatchRun(void);

#endif // DEBUGPRIMITIVE_H_