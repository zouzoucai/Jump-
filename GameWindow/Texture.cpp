/*==============================================================================

	テクスチャ処理　[Texture.cpp]

													Author : Sai Hakusen
													Date   : 2020 / 12 / 15
==============================================================================*/
#include "Texture.h"

#include <d3dx9.h>

#include <string.h>

#include "Mydirect3d.h"
#include "debugPrintf.h"

#define TEXTURE_MAX (1024)	// テクスチャ管理最大数

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// テクスチャ管理テーブル構造体
typedef struct Texture_tag
{
	LPDIRECT3DTEXTURE9 pTexture;
	unsigned long width;
	unsigned long height;
	char filename[TEXTURE_FILENAME_MAX];
}Texture;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
// テクスチャ管理テーブル
static Texture g_Textures[TEXTURE_MAX];

// 初期化の引数に動的メモリ確保で管理最大数とか入れると良い
void Texture_Initialize(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_Textures[i].pTexture = NULL;
		g_Textures[i].width = 0;
		g_Textures[i].height = 0;
		g_Textures[i].filename[0] = 0;
	}
}

// 基本使わないけど動的メモリ確保やるならここで後片付け
void Texture_Finalize(void)
{

}

// テクスチャファイルの読み込み予約
// 引数 pFileName...ファイル名
// 戻り値 テクスチャ管理番号...最大管理数を超えていた場合はINVALID_TEXTURE_ID
int Texture_SetTextureLoadFile(const char* pFileName)
{
	// ファイル名検索
	for (int i = 0; i < TEXTURE_MAX; i++) {
		// テーブルが空いていたらスキップ
		if (g_Textures[i].filename[0] == 0) {
			continue;
		}

		// 既に指定のファイルが予約されているかを調べる
		if (strcmp(pFileName, g_Textures[i].filename) == 0) {
			// 予約されている！
			return i;
		}
	}

	// 新規予約
	for (int i = 0; i < TEXTURE_MAX; i++) {
		// テーブルが使用中ならスキップ
		if (g_Textures[i].filename[0] != 0) {
			continue;
		}

		strcpy(g_Textures[i].filename, pFileName);

		return i;
	}

	return TEXTURE_INVALID_ID;
}

// テクスチャの読み込み
// 予約されているファイルを読み込みます
// 戻り値 読み込めなかったファイル数または他の不具合が発生した場合-1
// 
int Texture_Load(void)
{
	int err_count = 0;

	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	if (!pDevice) {
		return -1;
	}

	for (int i = 0; i < TEXTURE_MAX; i++) {

		if (g_Textures[i].filename[0] == 0) {
			// 空のテーブルなのでスキップ
			continue;
		}

		if (g_Textures[i].pTexture != NULL) {
			// 既に読み込まれている
			continue;
		}

		// テクスチャの読み込み
		HRESULT hr = D3DXCreateTextureFromFile(pDevice, g_Textures[i].filename, &g_Textures[i].pTexture);
		if (FAILED(hr)) {
			// テクスチャの読み込みに失敗
			DebugPrintf("読み込めなかったテクスチャファイル:[%s]\n", g_Textures[i].filename);
			err_count++;
		}
		else {
			// テクスチャの解像度(幅・高さ)の取得
			D3DXIMAGE_INFO info;
			D3DXGetImageInfoFromFile(g_Textures[i].filename, &info);
			g_Textures[i].width = info.Width;
			g_Textures[i].height = info.Height;
		}
	}

	return err_count;
}

// int textureIds[] => int* pTextureIds
// テクスチャの部分開放(メモリをあまり食わないようにする)
// 引数 textureIds[]...開放したいtextureIdが入ったint型配列の先頭アドレス
//      count...開放するテクスチャの数
void Texture_Release(int textureIds[], int count)
{
	for (int i = 0; i < count; i++) {
		if (g_Textures[textureIds[i]].pTexture) {
			g_Textures[textureIds[i]].pTexture->Release();
			g_Textures[textureIds[i]].pTexture = NULL;
		}
		g_Textures[textureIds[i]].filename[0] = 0;
		g_Textures[textureIds[i]].width = 0;
		g_Textures[textureIds[i]].height = 0;
	}

}

// テクスチャの全開放
void Texture_AllRelease(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Textures[i].pTexture) {
			g_Textures[i].pTexture->Release();
			g_Textures[i].pTexture = NULL;
		}
		g_Textures[i].filename[0] = 0;
		g_Textures[i].width = 0;
		g_Textures[i].height = 0;
	}
}

unsigned long Texture_GetTextureWidth(int textureId)
{
	return g_Textures[textureId].width;
}

unsigned long Texture_GetTextureHeight(int textureId)
{
	return g_Textures[textureId].height;
}

// テクスチャインタフェースポインタの取得
// 読み込めていないまたは不正な管理番号だった場合はNULL
LPDIRECT3DTEXTURE9 Texture_GetTexture(int textureId)
{
	if (!g_Textures[textureId].pTexture) {
		return NULL;
	}

	return g_Textures[textureId].pTexture;
}