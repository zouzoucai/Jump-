/*==============================================================================

	デバッグフォント表示処理　[debug_font.h]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#ifndef DEBUG_FONT_H_
#define DEBUG_FONT_H_

void DebugFont_Initialize(void);

void DebugFont_Finalize(void);

// デバッグフォントの表示
// 引数　dx, dy :デバッグ文字列の描画座標pString:デバッグ文字列へのポインタ
//       pString:デバッグ文字列へのポインタ
void DebugFont_Draw(float dx, float dy, const char* pString);


#endif // DEBUG_FONT_H_