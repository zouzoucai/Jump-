/*==============================================================================

	����t�H���g�\�������@[my_font.h]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 09
==============================================================================*/
#ifndef MY_FONT_H_
#define MY_FONT_H_

void MyFont_Initialize(void);
void MyFont_Finalize(void);

// ����t�H���g�̕\��
// �����@dx, dy :������̕`����W
//       dw, dh :������̕��A����
//       pString:������ւ̃|�C���^
//       color  :������̐F
void MyFont_Draw(float dx, float dy, float dw, float dh, const char* pString, D3DCOLOR color);


#endif // MY_FONT_H_