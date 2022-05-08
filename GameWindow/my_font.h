/*==============================================================================

	自作フォント表示処理　[my_font.h]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 09
==============================================================================*/
#ifndef MY_FONT_H_
#define MY_FONT_H_

void MyFont_Initialize(void);
void MyFont_Finalize(void);

// 自作フォントの表示
// 引数　dx, dy :文字列の描画座標
//       dw, dh :文字列の幅、高さ
//       pString:文字列へのポインタ
//       color  :文字列の色
void MyFont_Draw(float dx, float dy, float dw, float dh, const char* pString, D3DCOLOR color);


#endif // MY_FONT_H_