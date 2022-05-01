/*==============================================================================

	ゲーム処理　[Game.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 27
==============================================================================*/
#include <d3dx9.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Game.h"
#include "Game_Player.h"
#include "Game_Screw.h"
#include "Game_Judgment.h"
#include "Game_Scaffold.h"
#include "Game_Bg.h"
#include "Game_Goal.h"
#include "Game_Pause.h"
#include "Game_Timer.h"
#include "Fade.h"
#include "Scene.h"
#include "Controller.h"
#include "Grid.h"
#include "sound.h"
#include "StageSelect.h"
#include "Result.h"
typedef enum GameMode_tag
{
	GAME_FADE_IN,
	GAME_PLAY,
	GAME_PAUSE,
	GAME_CLEAR,
	GAME_FADE_OUT
}GameMode;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureId = TEXTURE_INVALID_ID;

static int g_FrameCounter = 0;

static bool g_ClearFlag = false;

static GameMode g_GameMode;

static Scene g_NextScene = SCENE_RESULT;


void Game_Initialize(void)
{
	GameBg_Initialize();
	GamePlayer_Initialize();
	GameScrew_Initialize();
	GameScaffold_Initialize();
	GameJudgment_Initialize();
	GameGoal_Initialize();
	GamePause_Initialize();
	Grid_Initialize();

	g_GameMode = GAME_FADE_IN;
	Fade_Start(FADE_IN, 0.0f, 0.0f, 0.0f, 30);
	g_NextScene = SCENE_RESULT;

	g_FrameCounter = 0;

	g_ClearFlag = false;
	
	GameTimer_Initialize();

	PlaySound(SOUND_LABEL_BGM_GAME);
}

void Game_Finalize(void)
{
	Grid_Finalize();
	GameTimer_Finalize();
	GamePause_Finalize();
	GameGoal_Finalize();
	GameScaffold_Finalize();
	GamePlayer_Finalize();
	GameScrew_Finalize();
	GameBg_Finalize();

	StopSound(SOUND_LABEL_BGM_GAME);
}

void Game_Update(void)
{
	switch (g_GameMode)
	{
	case GAME_FADE_IN:
		if (!Fade_IsFade()) {
			g_GameMode = GAME_PLAY;
		}
		break;
	case GAME_PLAY:
		Grid_Update();
		GameScrew_Update();
		GameBg_Update();
		GamePlayer_Update();
		GameScaffold_Update();
		GameJudgment_Update();
		GameTimer_Update();
		// 時間切れの場合はクリアできないようにする(今は時間切れ無し)
		/*if (!GameTime_isRemain()) {
			break;
		}*/
		if (GameGoal_Update()) {
			StageSelect_StageClear(GameGoal_GetNowStage());
			g_GameMode = GAME_CLEAR;
			GameTimer_Clear();
			Result_GetRank(GameTimer_GetNowRank());
			StageSelect_SetRank(GameTimer_GetRank());
			PlaySound(SOUND_LABEL_SE_DOOR_IN);
		}
		if (Controller_Menu()) {
			GamePause_Set();
			g_GameMode = GAME_PAUSE;
			StopSound(SOUND_LABEL_BGM_GAME);
			PlaySound(SOUND_LABEL_SE_PAUSE);
		}
		if (Controller_Retry()) {
			Game_Retry();
		}
		break;
	case GAME_PAUSE:
		GamePause_Update();
		switch (GamePause_GetSelect())
		{
		case PAUSE_NONE:
			break;
		case PAUSE_BACK:
			g_GameMode = GAME_PLAY;
			PlaySound(SOUND_LABEL_BGM_GAME);
			break;
		case PAUSE_RETRY:
			Game_Retry();
			break;
		case PAUSE_TOSTAGESELECT:
			Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 30);
			g_NextScene = SCENE_STAGESELECT;
			g_GameMode = GAME_FADE_OUT;
			break;
		case PAUSE_TOMENU:
			Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 30);
			g_NextScene = SCENE_MENU;
			g_GameMode = GAME_FADE_OUT;
			break;
		}
		break;
	case GAME_CLEAR:
		Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 60);
		g_NextScene = SCENE_RESULT;
		g_GameMode = GAME_FADE_OUT;
		break;
	case GAME_FADE_OUT:
		if (!Fade_IsFade()) {
			// 次のシーンへ移行
			Scene_Change(g_NextScene);
		}
		break;
	}

	g_FrameCounter++;
}

void Game_Draw(void)
{
	GameBg_Draw();
	GameGoal_Draw();
	GameScrew_Draw();
	GameScaffold_Draw();
	GamePlayer_Draw();
	Grid_Draw();
	GameTimer_Draw();
	GameBg_DrawStageNum();
	if (g_GameMode == GAME_PAUSE) {
		GamePause_Draw();
	}
}

void Game_Retry(void)
{
	GameBg_Reset();
	GamePlayer_Reset();
	GameScrew_Reset();
	GameScaffold_Reset();
	//GameJudgment_Initialize();
	// ゴールは同じステージなら位置変わらないのでfalseにするだけでOK
	GameGoal_ToDisenable();
	//GamePause_Initialize();

	g_GameMode = GAME_FADE_IN;
	Fade_Start(FADE_IN, 0.0f, 0.0f, 0.0f, RESET_FRAME);
	g_NextScene = SCENE_RESULT;

	g_FrameCounter = 0;

	g_ClearFlag = false;

	GameTimer_Reset();

	PlaySound(SOUND_LABEL_BGM_GAME);
}
