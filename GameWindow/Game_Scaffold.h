/*==============================================================================

	ゲームの足場処理　[Game_Scaffold.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 21
==============================================================================*/
#ifndef GAME_SCAFFOLD_H_
#define GAME_SCAFFOLD_H_

#include "Game_Screw.h"

typedef enum ScaffoldType_tag {
	TYPE_NONE,				// 未使用
	TYPE_BLOCK_ON,			// ブロック（ON）
	TYPE_BLOCK_OFF,			// ブロック（OFF）
	TYPE_FLOOR,				// 床
}ScaffoldType;

#define SCAFFOLD_MAX	(SCREW_MAX)
#define SCAFFOLD_SIZE	(SCREW_SIZE)
#define FLOOR_HEIGHT	(SCAFFOLD_SIZE / 5)

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// 足場構造体
typedef struct Scaffold_tag
{
	D3DXVECTOR2 position;			// 座標
	D3DXVECTOR2 initial_position;	// 初期座標（動かすときの基準）
	float width;					// 幅
	float height;					// 高さ
	int numHorizontal;				// 横の数
	int numVertical;				// 縦の数(床はどの数字入れても1で固定)
	ScaffoldType type;				// タイプ
	GimmickColor gimmick_color;		// ギミックカラー
	int texture_id;					// テクスチャID（ギミックカラー、オンかオフかによってテクスチャが異なるため）
	int texture_id_off;				// テクスチャID（ギミックカラー、オンかオフかによってテクスチャが異なるため）
}Scaffold;

void GameScaffold_Initialize(void);
void GameScaffold_Finalize(void);
void GameScaffold_Update(void);
void GameScaffold_Draw(void);

Scaffold* GameScaffold_GetScaffold(int i);

size_t GameScaffold_GetCount(void);

// gimmickColorに応じた足場のオンオフを切り替える
void GameScaffold_Switch(GimmickColor gimmickColor);

// gimmickColorに応じた足場を動かす
void GameScaffold_Move(GimmickColor gimmickColor, float tightness);

void GameScaffold_Reset(void);

#endif // GAME_SCAFFOLD_H_