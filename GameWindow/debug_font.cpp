/*==============================================================================

	�f�o�b�O�t�H���g�\�������@[debug_font.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 05
==============================================================================*/
#include <d3dx9.h>

#include "Texture.h"
#include "Sprite.h"
#include <string.h>

#define DEBUG_FONT_WIDTH (32)				// �f�o�b�O�t�H���g�̉���
#define DEBUG_FONT_HEIGHT (64)				// �f�o�b�O�t�H���g�̍���
#define DEBUG_FONT_ONE_LINE_CHAR_COUNT (16)	// �f�o�b�O�t�H���g�̈��Ɋ܂܂�镶����

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static int g_TextureId = TEXTURE_INVALID_ID;

void DebugFont_Initialize(void)
{
	// �e�N�X�`���̓ǂݍ��ݗ\��
	g_TextureId = Texture_SetTextureLoadFile("asset/texture/debug_font_32x64.png");

	// �e�N�X�`���̓ǂݍ���
	if (Texture_Load() > 0) {
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B", "�G���[", MB_OK);
	}
}

void DebugFont_Finalize(void)
{
	Texture_Release(&g_TextureId, 1);
}

// �f�o�b�O�t�H���g�̕\��
// �����@dx, dy :�f�o�b�O������̕`����WpString:�f�o�b�O������ւ̃|�C���^
//       pString:�f�o�b�O������ւ̃|�C���^
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