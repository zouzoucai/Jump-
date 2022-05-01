/*==============================================================================

	����t�H���g�\�������@[my_font.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 09
==============================================================================*/
#include <d3dx9.h>

#include "Texture.h"
#include "Sprite.h"
#include <string.h>

#define MY_FONT_WIDTH (64)					// ����t�H���g�̉���
#define MY_FONT_HEIGHT (128)				// ����t�H���g�̍���
#define MY_FONT_ONE_LINE_CHAR_COUNT (16)	// ����t�H���g�̈��Ɋ܂܂�镶����

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static int g_TextureId = TEXTURE_INVALID_ID;

void MyFont_Initialize(void)
{
	// �e�N�X�`���̓ǂݍ��ݗ\��
	g_TextureId = Texture_SetTextureLoadFile("asset/texture/my_font_64x128.png");

	// �e�N�X�`���̓ǂݍ���
	if (Texture_Load() > 0) {
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B", "�G���[", MB_OK);
	}
}

void MyFont_Finalize(void)
{
	Texture_Release(&g_TextureId, 1);
}

// ����t�H���g�̕\��
// �����@dx, dy :������̕`����W
//       dw, dh :������̕��A����
//       pString:������ւ̃|�C���^
//       color  :������̐F
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