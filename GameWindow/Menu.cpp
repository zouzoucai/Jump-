
/*==============================================================================

	タイトル処理　[Menu.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 14
==============================================================================*/
#include <d3dx9.h>
#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Keylogger.h"
#include "Scene.h"
#include "Fade.h"
#include "Scene.h"
#include "Controller.h"
#include "sound.h"

typedef enum MenuMode_tag
{
	MENU_FADE_IN,
	MENU_SHOW,
	MENU_FADE_OUT
}MenuMode;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureIdMenuBg = TEXTURE_INVALID_ID;//メニュー画面の背景
static int g_TextureIdMenuStart = TEXTURE_INVALID_ID;//ステージ選択のメニューバー
static int g_TextureIdMenuExplanation = TEXTURE_INVALID_ID;//操作説明のメニューバー
static int g_TextureIdMenuSound = TEXTURE_INVALID_ID;//音声調整メニューバー
static int g_TextureIdMenuMember = TEXTURE_INVALID_ID;//メンバー紹介メニューバー
static int g_TextureIdMenuEnd = TEXTURE_INVALID_ID;//エンドメニューバー(//ウィンドウハンドルの取得方法がわかんないですので、ゲームの終了は作ってない)
static int g_TextureIdMenuMark = TEXTURE_INVALID_ID;//マーク
static int g_TextureIdTheStart = TEXTURE_INVALID_ID;//ステージ選択
static int g_TextureIdTheExplanation = TEXTURE_INVALID_ID;//操作説明
//static int g_TextureIdTheSound = TEXTURE_INVALID_ID;//音声調整
static int g_TextureIdTheMember = TEXTURE_INVALID_ID;//メンバー紹介
static int g_TextureIdMenuText = TEXTURE_INVALID_ID;//メニューの文字
static int g_MenuCount = 0;
static D3DXVECTOR2 g_MenuMarkPosition = D3DXVECTOR2(0.0f, 0.0f);

//メニューバーの判定
static bool g_goToStart = false;
static bool g_goToExplanation = false;
static bool g_goToSound = false;
static bool g_goToMember = false;
static bool g_backToTitle = false;

static MenuMode g_MenuMode;

void Menu_Initialize(void)
{

	// テクスチャの読み込み予約
	g_TextureIdMenuBg = Texture_SetTextureLoadFile("asset/texture/menu_bg.png");
	g_TextureIdMenuStart = Texture_SetTextureLoadFile("asset/texture/すたーと.png");
	g_TextureIdMenuExplanation = Texture_SetTextureLoadFile("asset/texture/せつめい.png");
	//g_TextureIdMenuSound = Texture_SetTextureLoadFile("asset/texture/.png");
	g_TextureIdMenuMember = Texture_SetTextureLoadFile("asset/texture/めんばー.png");
	//g_TextureIdMenuEnd = Texture_SetTextureLoadFile("asset/texture/おわり.png");
	g_TextureIdMenuMark = Texture_SetTextureLoadFile("asset/texture/screwmark02.png");
	g_TextureIdTheStart = Texture_SetTextureLoadFile("asset/texture/field.png");
	g_TextureIdTheExplanation = Texture_SetTextureLoadFile("asset/texture/操作説明.png");
	//g_TextureIdTheSound = Texture_SetTextureLoadFile("asset/texture/field.png");
	g_TextureIdTheMember = Texture_SetTextureLoadFile("asset/texture/field.png");
	//g_TextureIdMenuText = Texture_SetTextureLoadFile("asset/texture/title_text.png");

	g_MenuMarkPosition = D3DXVECTOR2(360.0f, 290.0f);
	g_MenuCount = 0;
	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}

	g_goToStart = false;
	g_goToExplanation = false;
	g_goToSound = false;
	g_goToMember = false;
	g_backToTitle = false;

	g_MenuMode = MENU_FADE_IN;
	Fade_Start(FADE_IN, 0.0f, 0.0f, 0.0f, 60);

	PlaySound(SOUND_LABEL_BGM_MENU);
}

void Menu_Finalize(void)
{

	Texture_Release(&g_TextureIdMenuBg, 1);
	Texture_Release(&g_TextureIdMenuStart, 1);
	Texture_Release(&g_TextureIdMenuExplanation, 1);
	Texture_Release(&g_TextureIdMenuMember, 1);
	Texture_Release(&g_TextureIdMenuMark, 1);
	Texture_Release(&g_TextureIdTheStart, 1);
	Texture_Release(&g_TextureIdTheExplanation, 1);
	Texture_Release(&g_TextureIdTheMember, 1);

	StopSound(SOUND_LABEL_BGM_MENU);
}

void Menu_Update(void)
{
	switch (g_MenuMode)
	{
	case MENU_FADE_IN:
		if (!Fade_IsFade()) {
			g_MenuMode = MENU_SHOW;
		}
		break;
	case MENU_SHOW:
		//↑　↓とDECISIONキーでメニューバーを選ぶ
		if (Controller_Down() && g_MenuCount < 2)
		{
			g_MenuCount++;
			PlaySound(SOUND_LABEL_SE_SELECT);
		}
		if (Controller_Up() && g_MenuCount > 0)
		{
			g_MenuCount--;
			PlaySound(SOUND_LABEL_SE_SELECT);

		}
		if (g_MenuCount == 0)
		{
			//g_MenuMarkPosition.x = 730.0f;
			g_MenuMarkPosition.y = 290.0f;
			if (!g_goToStart && Controller_Decision())
			{
				g_backToTitle = false;
				g_goToStart = true;
				g_MenuMode = MENU_FADE_OUT;
				Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 60);
				PlaySound(SOUND_LABEL_SE_DECISION);

			}
			else if (Controller_Decision())
			{
				g_goToStart = false;
				g_backToTitle = true;
				PlaySound(SOUND_LABEL_SE_DECISION);
			}

		}
		if (g_MenuCount == 1)
		{
			//g_MenuMarkPosition.x = 730.0f;
			g_MenuMarkPosition.y = 450.0f;
			if (!g_goToExplanation && Controller_Decision())
			{
				g_backToTitle = false;
				g_goToExplanation = true;
				PlaySound(SOUND_LABEL_SE_DECISION);

			}
			else if (Controller_Decision())
			{
				g_goToExplanation = false;
				g_backToTitle = true;
				PlaySound(SOUND_LABEL_SE_DECISION);
			}

		}		
		if (g_MenuCount == 2)
		{
			g_MenuMarkPosition.y = 610.0f;
			if (!g_goToMember && Controller_Decision())
			{
				g_backToTitle = false;
				g_goToMember = true;
				PlaySound(SOUND_LABEL_SE_DECISION);

			}
			else if (Controller_Decision())
			{
				g_goToMember = false;
				g_backToTitle = true;
				PlaySound(SOUND_LABEL_SE_DECISION);
			}

		}		
		break;
	case MENU_FADE_OUT:
		if (!Fade_IsFade()) {
			// 次のシーンへ移行
			Scene_Change(SCENE_STAGESELECT);
		}
		break;
	}
	

}

void Menu_Draw(void)
{

	//各テクスチャの座標
	Sprite_Draw(g_TextureIdMenuBg, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	Sprite_Draw(g_TextureIdMenuMark, g_MenuMarkPosition.x, g_MenuMarkPosition.y, 150.0f, 60.0f, 0, 0, 570, 350);
	Sprite_Draw(g_TextureIdMenuStart,       44.0f, 270.0f, 340.0f, 130.0f, 0, 0, 512, 256);
	Sprite_Draw(g_TextureIdMenuExplanation, 44.0f, 420.0f, 340.0f, 130.0f, 0, 0, 512, 256);
	//Sprite_Draw(g_TextureIdMenuSound,     44.0f, 420.0f, 340.0f, 130.0f, 0, 0, 512, 256);
	Sprite_Draw(g_TextureIdMenuMember,      44.0f, 570.0f, 340.0f, 130.0f, 0, 0, 512, 256);
	//Sprite_Draw(g_TextureIdMenuEnd,         50.0f, 590.0f, 340.0f, 120.0f, 0, 0, 512, 256);
	//Sprite_Draw(g_TextureIdMenuText, 280.0f, 290.0f, 200.0f, 420.0f, 0, 0, 210, 420);


	if (g_goToStart == true)
	{
		//Sprite_Draw(g_TextureIdTheStart, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		//Scene_Change(SCENE_GAME);
	}
	if (g_goToExplanation == true)
	{
		Sprite_Draw(g_TextureIdTheExplanation, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 5334, 3001);
	}
	if (g_goToSound == true)
	{
		//Sprite_Draw(g_TextureIdTheSound, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	if (g_goToMember == true)
	{
		Sprite_Draw(g_TextureIdTheMember, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	if (g_backToTitle == true)
	{
		Sprite_Draw(g_TextureIdMenuBg, 0.0f, 0.0f, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		Sprite_Draw(g_TextureIdMenuMark, g_MenuMarkPosition.x, g_MenuMarkPosition.y, 150.0f, 60.0f, 0, 0, 570, 350);
		Sprite_Draw(g_TextureIdMenuStart,       44.0f, 270.0f, 340.0f, 130.0f, 0, 0, 512, 256);
		Sprite_Draw(g_TextureIdMenuExplanation, 44.0f, 420.0f, 340.0f, 130.0f, 0, 0, 512, 256);
		//Sprite_Draw(g_TextureIdMenuSound,     44.0f, 460.0f, 140.0f, 130.0f, 0, 0, 512, 256);
		Sprite_Draw(g_TextureIdMenuMember,      44.0f, 570.0f, 340.0f, 130.0f, 0, 0, 512, 256);
		//Sprite_Draw(g_TextureIdMenuEnd,         50.0f, 590.0f, 340.0f, 120.0f, 0, 0, 512, 256);
		//Sprite_Draw(g_TextureIdMenuText, 280.0f, 290.0f, 200.0f, 420.0f, 0, 0, 210, 420);
	}
}