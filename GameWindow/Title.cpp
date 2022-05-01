/*==============================================================================

	�^�C�g�������@[Title.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 22
==============================================================================*/
#include <d3dx9.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Keylogger.h"
#include "Scene.h"
#include "Fade.h"
#include "Scene.h"
#include <stdio.h>
#include "Controller.h"
#include "sound.h"

typedef enum TitleMode_tag
{
	TITLE_FADE_IN,
	TITLE_LOGO,
	TITLE_SHOW,
	TITLE_FADE_OUT
}TitleMode;

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static int g_TextureIdLogo = TEXTURE_INVALID_ID;//���S
static int g_TextureIdMainTitle = TEXTURE_INVALID_ID;//���C���^�C�g��
static int g_TextureIdSpace = TEXTURE_INVALID_ID;//�Ղꂷ���؁[��

static TitleMode g_TitleMode;

static bool g_isDispTitle = false;

static int g_FrameCounter = 0;

void Title_Initialize(void)
{

	// �e�N�X�`���̓ǂݍ��ݗ\��
	g_TextureIdLogo = Texture_SetTextureLoadFile("asset/texture/TeamLogo.png");
	g_TextureIdMainTitle = Texture_SetTextureLoadFile("asset/texture/title07.png");
	g_TextureIdSpace = Texture_SetTextureLoadFile("asset/texture/pressspace.png");
	
	// �e�N�X�`���̓ǂݍ���
	if (Texture_Load() > 0) {
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B", "�G���[", MB_OK);
		return;
	}

	g_isDispTitle = false;

	g_TitleMode = TITLE_FADE_IN;
	Fade_Start(FADE_IN, 0.0f, 0.0f, 0.0f, 60);
}

void Title_Finalize(void)
{
	Texture_Release(&g_TextureIdSpace, 1);
	Texture_Release(&g_TextureIdLogo, 1);
	Texture_Release(&g_TextureIdMainTitle, 1);

	StopSound(SOUND_LABEL_BGM_TITLE);
}

void Title_Update(void)
{
	switch (g_TitleMode)
	{
	case TITLE_FADE_IN:
		if (Controller_Decision()) {
			g_TitleMode = TITLE_FADE_OUT;
			Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 30);
			PlaySound(SOUND_LABEL_SE_DECISION);
		}
		if (!Fade_IsFade()) {
			if (!g_isDispTitle) {
				g_TitleMode = TITLE_LOGO;
			}
			else {
				g_TitleMode = TITLE_SHOW;
				PlaySound(SOUND_LABEL_BGM_TITLE);
			}
		}
		break;
	case TITLE_LOGO:
		if (Controller_Decision()) {
			g_TitleMode = TITLE_FADE_OUT;
			Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 60);
			PlaySound(SOUND_LABEL_SE_DECISION);
		}
		if (g_FrameCounter > 120) {
			g_TitleMode = TITLE_FADE_OUT;
			Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 60);
		}

		break;
	case TITLE_SHOW:
		if (Controller_Decision()) {
			g_TitleMode = TITLE_FADE_OUT;
			Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 60);
			PlaySound(SOUND_LABEL_SE_DECISION);
		}
		break;
	case TITLE_FADE_OUT:
		if (!Fade_IsFade()) {
			if (!g_isDispTitle) {
				g_TitleMode = TITLE_FADE_IN;
				g_isDispTitle = true;
				Fade_Start(FADE_IN, 0.0f, 0.0f, 0.0f, 60);
			}
			else {
				// ���̃V�[���ֈڍs
				Scene_Change(SCENE_MENU);
			}
		}
		break;
	}

	g_FrameCounter++;
}

void Title_Draw(void)
{
	if (!g_isDispTitle) {
		Sprite_Draw(g_TextureIdLogo, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	else {
		Sprite_Draw(g_TextureIdMainTitle, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		if (g_FrameCounter % 60 < 40) {
			//Sprite_Draw(g_TextureIdSpace, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}
	}
	

}