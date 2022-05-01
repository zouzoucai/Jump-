/*==============================================================================

	direct3Dデバイスの設定処理　[Mydirect3d.h]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#ifndef MY_DIRECT3D_H
#define MY_DIRECT3D_H

#include <d3d9.h>

// direct3d9の初期化
// 失敗したらfalseを返す
bool MyDirect3D_Initialize(HWND hWnd);

// direct3dpの終了処理
void MyDirect3D_Finalize(void);

// Direct3Dデバイスの取得
LPDIRECT3DDEVICE9 MyDirect3D_GetDevice(void);

#endif // MY_DIRECT3D_H