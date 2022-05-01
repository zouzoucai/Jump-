/*==============================================================================

	デバッグフォント表示処理　[debug_font.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 05
==============================================================================*/
#include <d3dx9.h>

#include "Texture.h"
#include "Sprite.h"
#include <string.h>

#define DEBUG_FONT_WIDTH (32)				// デバッグフォントの横幅
#define DEBUG_FONT_HEIGHT (64)				// デバッグフォントの高さ
#define DEBUG_FONT_ONE_LINE_CHAR_COUNT (16)	// デバッグフォントの一列に含まれる文字数

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureId = TEXTURE_INVALID_ID;

void DebugFont_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureId = Texture_SetTextureLoadFile("asset/texture/debug_font_32x64.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
	}
}

void DebugFont_Finalize(void)
{
	Texture_Release(&g_TextureId, 1);
}

// デバッグフォントの表示
// 引数　dx, dy :デバッグ文字列の描画座標pString:デバッグ文字列へのポインタ
//       pString:デバッグ文字列へのポインタ
void DebugFont_Draw(float dx, float dy, const char* pString)
{
	for (int i = 0; i < strlen(pString); i++) {
		int offset = pString[i] - ' ';
		int tcx = (offset % DEBUG_FONT_ONE_LINE_CHAR_COUNT) * DEBUG_FONT_WIDTH;
		int tcy = (offset / DEBUG_FONT_ONE_LINE_CHAR_COUNT) * DEBUG_FONT_HEIGHT;

		Sprite_Draw(g_TextureId, dx + i * DEBUG_FONT_WIDTH, dy, 
			tcx, tcy, DEBUG_FONT_WIDTH, DEBUG_FONT_HEIGHT);
	}
}