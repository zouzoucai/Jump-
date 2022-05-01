/*==============================================================================

	シーン処理　[Scene.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 10
==============================================================================*/
#include "Scene.h"
#include "Title.h"
#include "Menu.h"
#include "StageSelect.h"
#include "Game.h"
#include "Result.h"

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static Scene g_NowScene = SCENE_TITLE;
static Scene g_NextScene = g_NowScene;

// 関数ポインタ
typedef void(*SceneFunction)(void);

// ここの順番はenumに準拠
static const SceneFunction g_pSceneInitialize[]={
	Title_Initialize,
	Menu_Initialize,
	StageSelect_Initialize,
	Game_Initialize,
	Result_Initialize
};

static const SceneFunction g_pSceneFinalize[] = {
	Title_Finalize,
	Menu_Finalize,
	StageSelect_Finalize,
	Game_Finalize,
	Result_Finalize
};

static const SceneFunction g_pSceneUpdate[] = {
	Title_Update,
	Menu_Update,
	StageSelect_Update,
	Game_Update,
	Result_Update
};

static const SceneFunction g_pSceneDraw[] = {
	Title_Draw,
	Menu_Draw,
	StageSelect_Draw,
	Game_Draw,
	Result_Draw
};

void Scene_Initialize(void)
{
	g_pSceneInitialize[g_NowScene]();
}

void Scene_Finalize(void)
{
	g_pSceneFinalize[g_NowScene]();
}

void Scene_Update(void)
{
	g_pSceneUpdate[g_NowScene]();
}

void Scene_Draw(void)
{
	g_pSceneDraw[g_NowScene]();
}

void Scene_Change(Scene nextScene)
{
	g_NextScene = nextScene;
}

void Scene_ExecuteChange(void)
{
	// 今のシーンと次のシーンが違うものだったら
	if (g_NowScene == g_NextScene) return;

	// 現在のシーンを終了する
	Scene_Finalize();

	// 現在のシーンを次のシーンにする
	g_NowScene = g_NextScene;

	// 次のシーンの初期化をする
	Scene_Initialize();

}