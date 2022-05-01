/*==============================================================================

	�t�F�[�h�����@[Fade.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 05
==============================================================================*/
#include <d3dx9.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Fade.h"

static FadeMode g_FadeMode = FADE_NONE;
static D3DXCOLOR g_Color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
static float g_Prop = 0.0f;
static int g_FadeFrame = 0;
static int g_FrameCount = 0;
static int g_FadeStartFrame = 0;
static int g_TextureId = TEXTURE_INVALID_ID;

void Fade_Initialize(void)
{
	g_FadeMode = FADE_NONE;
	g_Color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	g_Prop = 0.0f;
	g_FadeFrame = 0;
	g_FrameCount = 0;
	g_FadeStartFrame = 0;
	g_TextureId = TEXTURE_INVALID_ID;

	// 8�~8�̔����e�N�X�`����ǂݍ���
	g_TextureId = Texture_SetTextureLoadFile("asset/texture/fade.png");
}

void Fade_Finalize(void)
{
	Texture_Release(&g_TextureId, 1);
}

void Fade_Update(void)
{
	g_FrameCount++;

	// �t�F�[�h���ĂȂ������牽�����Ȃ�
	if (g_FadeMode == FADE_NONE) return;

	// ���݂̃t�F�[�h�t���[�������Z�o
	int frame = g_FrameCount - g_FadeStartFrame;

	// �ŏI�t�F�[�h�t���[��������0�`1�̐i�s�������Z�o����
	g_Prop = (float)frame / g_FadeFrame;

	// �t�F�[�h�̐i�s������1�𒴂��Ă�����I��
	if (g_Prop > 1.0f) {
		g_FadeMode = FADE_NONE;
	}
}

void Fade_Draw(void)
{
	if (g_FadeMode != FADE_NONE) {
		g_Color.a = g_FadeMode == FADE_IN ? 1.0f - g_Prop : g_Prop;
	}

	if (g_Color.a <= 0.001f) return;

	Sprite_SetColor(g_Color);

	Sprite_Draw(g_TextureId, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 8, 8);

	Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
}

void Fade_Start(FadeMode mode, float r, float g, float b, int frame)
{
	g_FadeMode = mode;
	g_Color = D3DXCOLOR(r, g, b, 0.0f);
	g_Prop = 0.0f;
	g_FadeFrame = frame;
	g_FadeStartFrame = g_FrameCount;
}

bool Fade_IsFade(void)
{
	return g_FadeMode != FADE_NONE;
}