/*==============================================================================

	�V�[�������@[Scene.cpp]

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
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static Scene g_NowScene = SCENE_TITLE;
static Scene g_NextScene = g_NowScene;

// �֐��|�C���^
typedef void(*SceneFunction)(void);

// �����̏��Ԃ�enum�ɏ���
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
	// ���̃V�[���Ǝ��̃V�[�����Ⴄ���̂�������
	if (g_NowScene == g_NextScene) return;

	// ���݂̃V�[�����I������
	Scene_Finalize();

	// ���݂̃V�[�������̃V�[���ɂ���
	g_NowScene = g_NextScene;

	// ���̃V�[���̏�����������
	Scene_Initialize();

}