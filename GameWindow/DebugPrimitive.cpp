/*==============================================================================

	デバッグ用図形表示処理　[DebugPrimitive.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#if defined(_DEBUG) || defined(DEBUG)	// デバッグの時だけ実行

#include <d3dx9.h>
#include <math.h>
#include "Mydirect3d.h"

#define CIRCLE_VERTEX_COUNT	(64)	// 64角形
#define CIRCLE_DRAW_MAX		(2048)	// 2048個の64角形

typedef struct DebugVertex_tag
{
	D3DXVECTOR4 position;
	D3DCOLOR color;
}DebugVertex;
#define FVF_DEBUG_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

static LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
static LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;

// この3つは毎フレーム初期化される
static int g_CircleCount = 0;
static DebugVertex* g_pDebugVertex = NULL;
static WORD* g_pDebugVertexIndex = NULL;

#endif // _DEBUG || DEBUG

void DebugPrimitive_Initialize(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	// 各グローバル変数の初期化
	g_CircleCount = 0;

	// 頂点バッファやインデックスバッファの確保
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();

	pDevice->CreateVertexBuffer(
		sizeof(DebugVertex) * CIRCLE_VERTEX_COUNT * CIRCLE_DRAW_MAX,
		D3DUSAGE_WRITEONLY,		// 使い方(今回は書くだけで読まないから処理半分)
		FVF_DEBUG_VERTEX,		// FVF
		D3DPOOL_MANAGED,		// メモリの管理方法(DEFAULTだと自分たちで復旧させない限り絵が出ないので注意)
		&g_pVertexBuffer,		// 取得したインターフェースのアドレスを記録するためのポインタへのアドレス
		NULL
	);

	pDevice->CreateIndexBuffer(
		sizeof(WORD) * CIRCLE_VERTEX_COUNT * 2 * CIRCLE_DRAW_MAX,
		D3DUSAGE_WRITEONLY,		// 使い方(今回は書くだけで読まないから処理半分)
		D3DFMT_INDEX16,			// FVF
		D3DPOOL_MANAGED,		// メモリの管理方法(DEFAULTだと自分たちで復旧させない限り絵が出ないので注意)
		&g_pIndexBuffer,
		NULL
	);

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_Finalize(void)
{
#if defined(_DEBUG) || defined(DEBUG)

	if (g_pIndexBuffer) {
		g_pIndexBuffer->Release();
		g_pIndexBuffer = NULL;
	}

	if (g_pVertexBuffer) {
		g_pVertexBuffer->Release();
		g_pVertexBuffer = NULL;
	}

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_BatchBegin(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	// 頂点バッファと頂点インデックスバッファをロックする
	g_pVertexBuffer->Lock(0, 0, (void**)&g_pDebugVertex, 0);
	g_pIndexBuffer->Lock(0, 0, (void**)&g_pDebugVertexIndex, 0);

	// 丸の描画命令数を初期化
	g_CircleCount = 0;

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_BatchDrawCircle(float x, float y, float radius)
{
#if defined(_DEBUG) || defined(DEBUG)
	// 指定座標を円の中心とした丸の頂点座標を計算して、
	// 頂点バッファと頂点インデックスバッファに書き入れる

	// 配列のどこから書き入れるか
	WORD n = (WORD)(g_CircleCount * CIRCLE_VERTEX_COUNT);

	// 三角形一個分の（ラジアン角）
	float s = D3DX_PI * 2.0f / CIRCLE_VERTEX_COUNT;

	// 頂点バッファと頂点インデックスバッファへ情報を書き込む
	for (WORD i = 0; i < CIRCLE_VERTEX_COUNT; i++) {
		g_pDebugVertex[n + i].position.y = sinf(s * i) * radius + y;
		g_pDebugVertex[n + i].position.x = cosf(s * i) * radius + x;
		g_pDebugVertex[n + i].position.z = 1.0f;
		g_pDebugVertex[n + i].position.w = 1.0f;
		g_pDebugVertex[n + i].color = D3DCOLOR_RGBA(0, 0, 0, 255); //色の指定引数で決められるようにするのもアリ

		g_pDebugVertexIndex[n * 2 + i * 2] = n + i;
		g_pDebugVertexIndex[n * 2 + i * 2 + 1] = n + (i + 1) % CIRCLE_VERTEX_COUNT;
	}

	// 丸の描画命令数を増やす
	g_CircleCount++;

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_BatchRun(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	// 頂点バッファと頂点インデックスバッファをアンロックする
	g_pVertexBuffer->Unlock();
	g_pIndexBuffer->Unlock();

	// 丸の描画命令数分の丸を描く
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	
	int vn = g_CircleCount * CIRCLE_VERTEX_COUNT;

	pDevice->SetFVF(FVF_DEBUG_VERTEX);
	pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(DebugVertex));
	pDevice->SetIndices(g_pIndexBuffer);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, vn, 0, vn);
#endif // _DEBUG || DEBUG
}