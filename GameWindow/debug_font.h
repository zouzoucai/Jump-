/*==============================================================================

	�f�o�b�O�t�H���g�\�������@[debug_font.h]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#ifndef DEBUG_FONT_H_
#define DEBUG_FONT_H_

void DebugFont_Initialize(void);

void DebugFont_Finalize(void);

// �f�o�b�O�t�H���g�̕\��
// �����@dx, dy :�f�o�b�O������̕`����WpString:�f�o�b�O������ւ̃|�C���^
//       pString:�f�o�b�O������ւ̃|�C���^
void DebugFont_Draw(float dx, float dy, const char* pString);


#endif // DEBUG_FONT_H_