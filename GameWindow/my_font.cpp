/*==============================================================================

	自作フォント表示処理　[my_font.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 09
==============================================================================*/
#include <d3dx9.h>

#include "Texture.h"
#include "Sprite.h"
#include <string.h>

#define MY_FONT_WIDTH (64)					// 自作フォントの横幅
#define MY_FONT_HEIGHT (128)				// 自作フォントの高さ
#define MY_FONT_ONE_LINE_CHAR_COUNT (16)	// 自作フォントの一列に含まれる文字数

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureId = TEXTURE_INVALID_ID;

void MyFont_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureId = Texture_SetTextureLoadFile("asset/texture/my_font_64x128.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
	}
}

void MyFont_Finalize(void)
{
	Texture_Release(&g_TextureId, 1);
}

// 自作フォントの表示
// 引数　dx, dy :文字列の描画座標
//       dw, dh :文字列の幅、高さ
//       pString:文字列へのポインタ
//       color  :文字列の色
void MyFont_Draw(float dx, float dy, float dw, float dh, const char* pString, D3DCOLOR color)
{
	for (int i = 0; i < strlen(pString); i++) {
		int offset = pString[i] - ' ';
		int tcx = (offset % MY_FONT_ONE_LINE_CHAR_COUNT) * MY_FONT_WIDTH;
		int tcy = (offset / MY_FONT_ONE_LINE_CHAR_COUNT) * MY_FONT_HEIGHT;

		Sprite_Draw(g_TextureId, dx + i * dw, dy, dw, dh,
			tcx, tcy, MY_FONT_WIDTH, MY_FONT_HEIGHT, color);
	}
}