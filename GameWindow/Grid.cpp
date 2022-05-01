/*==============================================================================

	グリッド描画 [Grid.cpp]
													Author : Ryuichi Tsunoda
													Date   : 2020 / 12 / 12
==============================================================================*/
#if defined(_DEBUG) || defined(DEBUG)	// デバッグの時だけ実行
#include "Mydirect3d.h"
#include "Grid.h"
#include <stdlib.h>
#include "Game_Screw.h"

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// 頂点構造体
typedef struct VERTEX2D_tag
{
	D3DXVECTOR2 Position;
	D3DCOLOR	Color;
}Vertex2D;
#define FVF_GRID_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static Vertex2D* g_pVertex = NULL;
static int g_VertexCount = 0;
static int g_LineCount = 0;

#endif // _DEBUG || DEBUG

void Grid_Initialize(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	// 両サイドに幅10.0fの余白があり、そこにも横のグリッドを入れるためcount_xに+2している
	int count_x = SCREW_MAX_X + 2;
	int count_y = SCREW_MAX_Y;
	float size = SCREW_SIZE;

	g_LineCount = (count_x + 1) + (count_y + 1);
	g_VertexCount = g_LineCount * 2;

	g_pVertex = (Vertex2D*)malloc(g_VertexCount * sizeof(Vertex2D));

	float length_x = size * count_x;
	float length_y = size * count_y;
	int n = (count_x + 1) * 2;

	for (int i = 0; i < g_VertexCount; i++) {
		g_pVertex[i].Color = D3DCOLOR_RGBA(0, 0, 0, 255);
	}

	for (int i = 0; i < count_x + 1; i++) {
		g_pVertex[    i * 2    ].Position = D3DXVECTOR2(START_X + size * i, 0.0f);
		g_pVertex[    i * 2 + 1].Position = D3DXVECTOR2(START_X + size * i, length_y);
	}
	for (int i = 0; i < count_y + 1; i++) {
		g_pVertex[n + i * 2].Position = D3DXVECTOR2(START_X, size * i);
		g_pVertex[n + i * 2 + 1].Position = D3DXVECTOR2(length_x, size * i);
	}
#endif // _DEBUG || DEBUG
}

void Grid_Finalize(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	free(g_pVertex);
#endif // _DEBUG || DEBUG
}

void Grid_Update(void)
{
	
}

void Grid_Draw(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();

	// デバイスのFVF設定
	pDevice->SetFVF(FVF_GRID_VERTEX);

	// デバイスにテクスチャの設定をする
	pDevice->SetTexture(0, NULL);

	// ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, g_LineCount, g_pVertex, sizeof(Vertex2D));
#endif // _DEBUG || DEBUG
}