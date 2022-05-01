/*==============================================================================

	ÉVÅ[ÉìèàóùÅ@[Scene.h]

													Author : Sai Hakusen
													Date   : 2020 / 12 / 25
==============================================================================*/
#ifndef SCENE_H_
#define SCENE_H_

typedef enum Scene_tag {
	SCENE_TITLE,
	SCENE_MENU,
	SCENE_STAGESELECT,
	SCENE_GAME,
	SCENE_RESULT,
	SCENE_MAX,
}Scene;

void Scene_Initialize(void);
void Scene_Finalize(void);
void Scene_Update(void);
void Scene_Draw(void);

void Scene_Change(Scene nextScene);
void Scene_ExecuteChange(void);


#endif // SCENE_H_