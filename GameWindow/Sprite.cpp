/*==============================================================================

	スプライト処理　[Sprite.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 06
==============================================================================*/
#include <d3dx9.h>
#include "Mydirect3d.h"
#include "Texture.h"
#include "Sprite.h"

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// 頂点構造体
typedef struct VERTEX2D_tag
{
	D3DXVECTOR4 Position;
	D3DCOLOR	Color;
	D3DXVECTOR2 TexCoord;
}Vertex2D;
#define FVF_VERTEX2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;	// 頂点バッファのインターフェース
static LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;	// インデックスバッファのインターフェース

static D3DCOLOR g_color = 0xffffffff;					// ホワイト

void Sprite_Initialize(void)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		MessageBox(NULL, "デバイスの取得に失敗しました。", "エラー", MB_OK);
		return;
	}

	pDevice->CreateVertexBuffer(
		sizeof(Vertex2D) * 4,	// 頂点バッファの量(バイト)
		D3DUSAGE_WRITEONLY,		// 使い方(今回は書くだけで読まないから処理半分)
		FVF_VERTEX2D,			// FVF
		D3DPOOL_MANAGED,		// メモリの管理方法(DEFAULTだと自分たちで復旧させない限り絵が出ないので注意)
		&g_pVertexBuffer,		// 取得したインターフェースのアドレスを記録するためのポインタへのアドレス
		NULL
	);

	pDevice->CreateIndexBuffer(
		sizeof(WORD) * 6,		// インデックスバッファの量(バイト)
		D3DUSAGE_WRITEONLY,		// 使い方(今回は書くだけで読まないから処理半分)
		D3DFMT_INDEX16,			// フォーマット
		D3DPOOL_MANAGED,		// メモリの管理方法(DEFAULTだと自分たちで復旧させない限り絵が出ないので注意)
		&g_pIndexBuffer,		// 取得したインターフェースのアドレスを記録するためのポインタへのアドレス
		NULL
	);
}

void Sprite_Finalize(void)
{
	if (g_pIndexBuffer) {
		g_pIndexBuffer->Release();
		g_pIndexBuffer = NULL;
	}

	if (g_pVertexBuffer) {
		g_pVertexBuffer->Release();
		g_pVertexBuffer = NULL;
	}
}

// スプライトポリゴンカラー設定
// 引数:color	ポリゴンカラー
void Sprite_SetColor(D3DCOLOR color)
{
	g_color = color;

}

// スプライトの描画
// テクスチャサイズでの描画
void Sprite_Draw(int textureId, float dx, float dy)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		return;
	}

	// デバイスのFVF設定
	pDevice->SetFVF(FVF_VERTEX2D);

	// デバイスにテクスチャの設定をする
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// テクスチャのサイズ取得
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	// 頂点データの作成
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,     dy - 0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(0.0f,0.0f)},
		{D3DXVECTOR4(dx + w - 0.5f, dy - 0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(1.0f,0.0f)},
		{D3DXVECTOR4(dx - 0.5f,     dy + h - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(0.0f,1.0f)},
		{D3DXVECTOR4(dx + w - 0.5f, dy + h - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(1.0f,1.0f)},
	};

	// 頂点バッファのロックとデータの書き込み
	Vertex2D* pV;
	// 第一引数と第二引数が0なら全部ロックしてっていう指示になる
	g_pVertexBuffer->Lock(0, 0, (void**)&pV, 0);

	memcpy(pV, v, sizeof(v));

	g_pVertexBuffer->Unlock();

	// インデックスデータ
	//unsigned short index[] = {};
	WORD index[] = { 0,1,2,1,3,2 };

	// 頂点バッファのロックとデータの書き込み
	WORD* pI;
	// 第一引数と第二引数が0なら全部ロックしてっていう指示になる
	g_pIndexBuffer->Lock(0, 0, (void**)&pI, 0);
	pI[0] = 0;
	pI[1] = 1;
	pI[2] = 2;
	pI[3] = 1;
	pI[4] = 3;
	pI[5] = 2;

	g_pIndexBuffer->Unlock();

	// 頂点バッファの指定
	pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex2D));

	// インデックスバッファの指定
	pDevice->SetIndices(g_pIndexBuffer);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}

// スプライトの描画
// 切り取りサイズのポリゴンで描画
// 引数　tcx:テクスチャの切り取りx座標
// 引数　tcy:テクスチャの切り取りy座標
// 引数　tcw:テクスチャの切り取り幅
// 引数　tch:テクスチャの切り取り高さ
void Sprite_Draw(int textureId, float dx, float dy, int tcx, int tcy, int tcw, int tch)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		return;
	}

	// デバイスのFVF設定
	pDevice->SetFVF(FVF_VERTEX2D);

	// デバイスにテクスチャの設定をする
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// テクスチャのサイズ取得
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// 頂点データの作成
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,       dy +  - 0.5f,    1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dx + tcw - 0.5f, dy +  - 0.5f,    1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(dx - 0.5f,       dy + tch - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dx + tcw - 0.5f, dy + tch - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// ここが大切！！
	// 頂点バッファのロックとデータの書き込み
	Vertex2D* pV;
	// 第一引数と第二引数が0なら全部ロックしてっていう指示になる
	g_pVertexBuffer->Lock(0, 0, (void**)&pV, 0);

	memcpy(pV, v, sizeof(v));

	g_pVertexBuffer->Unlock();
	
	// 頂点バッファの指定
	pDevice->SetStreamSource(0,g_pVertexBuffer,0,sizeof(Vertex2D));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

// スプライトの描画
// ポリゴンの幅・高さを指定
// 引数　dw;ポリゴンサイズ幅
// 引数　dw;ポリゴンサイズ高さ
// 引数　tcx:テクスチャの切り取りx座標
// 引数　tcy:テクスチャの切り取りy座標
// 引数　tcw:テクスチャの切り取り幅
// 引数　tch:テクスチャの切り取り高さ
void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		return;
	}

	// デバイスのFVF設定
	pDevice->SetFVF(FVF_VERTEX2D);

	// デバイスにテクスチャの設定をする
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// テクスチャのサイズ取得
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// 頂点データの作成
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,      dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(dx - 0.5f,      dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// ポリゴンの描画（簡易版）
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));
}

// スプライトの描画
// ポリゴンの幅・高さを指定
// 引数　dw;ポリゴンサイズ幅
// 引数　dw;ポリゴンサイズ高さ
// 引数　tcx:テクスチャの切り取りx座標
// 引数　tcy:テクスチャの切り取りy座標
// 引数　tcw:テクスチャの切り取り幅
// 引数　tch:テクスチャの切り取り高さ
void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch, D3DCOLOR color)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		return;
	}

	// デバイスのFVF設定
	pDevice->SetFVF(FVF_VERTEX2D);

	// デバイスにテクスチャの設定をする
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// テクスチャのサイズ取得
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// 引数で指定した色に変える
	g_color = color;

	// 頂点データの作成
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,      dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(dx - 0.5f,      dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// ポリゴンの描画（簡易版）
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));

	// 色を元に戻す
	g_color = 0xffffffff;
}

// スプライトの描画
// ポリゴンの幅・高さを指定
// 引数　dw;ポリゴンサイズ幅
// 引数　dw;ポリゴンサイズ高さ
// 引数　tcx:テクスチャの切り取りx座標
// 引数　tcy:テクスチャの切り取りy座標
// 引数　tcw:テクスチャの切り取り幅
// 引数　tch:テクスチャの切り取り高さ
// 引数  cx:回転・拡大の中心のx座標
// 引数  cy:回転・拡大の中心のy座標
// 引数  angle:ポリゴンの回転角度
void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch,
	float cx, float cy, float angle)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		return;
	}

	// デバイスのFVF設定
	pDevice->SetFVF(FVF_VERTEX2D);



	// デバイスにテクスチャの設定をする
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// テクスチャのサイズ取得
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// 頂点データの作成
	Vertex2D v[] = {
		{D3DXVECTOR4(     0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dw - 0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(     0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dw - 0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// 平行移動行列の作成
	D3DXMATRIX mtxTranslationC;
	D3DXMatrixTranslation(&mtxTranslationC, -cx, -cy, 0.0f);

	// 元に戻す平行移動行列の作成
	D3DXMATRIX mtxTranslationI;
	D3DXMatrixTranslation(&mtxTranslationI, cx + dx, cy + dy, 0.0f);

	// 回転行列の作成
	D3DXMATRIX mtxRotation;
	D3DXMatrixRotationZ(&mtxRotation, angle);

	// 拡大行列の作成
	D3DXMATRIX mtxScale;
	D3DXMatrixScaling(&mtxScale, 1.0f, 1.0f, 1.0f);	// 1が等倍、負の値は裏表が逆になる

	// 行列の合成
	D3DXMATRIX mtxWorld;
	mtxWorld = mtxTranslationC * mtxScale * mtxRotation * mtxTranslationI;	// *は行列の合成を表す(順番には意味があるため要注意！！！！)

	// 座標変換
	for (int i = 0; i < 4; i++) {
		D3DXVec4Transform(&v[i].Position, &v[i].Position, &mtxWorld);
	}

	// ポリゴンの描画（簡易版）
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));
}

void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch, D3DCOLOR color,
	float cx, float cy, float angle)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		return;
	}

	// デバイスのFVF設定
	pDevice->SetFVF(FVF_VERTEX2D);



	// デバイスにテクスチャの設定をする
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// テクスチャのサイズ取得
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// 引数で指定した色に変える
	g_color = color;

	// 頂点データの作成
	Vertex2D v[] = {
		{D3DXVECTOR4(0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dw - 0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dw - 0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// 平行移動行列の作成
	D3DXMATRIX mtxTranslationC;
	D3DXMatrixTranslation(&mtxTranslationC, -cx, -cy, 0.0f);

	// 元に戻す平行移動行列の作成
	D3DXMATRIX mtxTranslationI;
	D3DXMatrixTranslation(&mtxTranslationI, cx + dx, cy + dy, 0.0f);

	// 回転行列の作成
	D3DXMATRIX mtxRotation;
	D3DXMatrixRotationZ(&mtxRotation, angle);

	// 拡大行列の作成
	D3DXMATRIX mtxScale;
	D3DXMatrixScaling(&mtxScale, 1.0f, 1.0f, 1.0f);	// 1が等倍、負の値は裏表が逆になる

	// 行列の合成
	D3DXMATRIX mtxWorld;
	mtxWorld = mtxTranslationC * mtxScale * mtxRotation * mtxTranslationI;	// *は行列の合成を表す(順番には意味があるため要注意！！！！)

	// 座標変換
	for (int i = 0; i < 4; i++) {
		D3DXVec4Transform(&v[i].Position, &v[i].Position, &mtxWorld);
	}

	// ポリゴンの描画（簡易版）
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));

	// 色を元に戻す
	g_color = 0xffffffff;
}