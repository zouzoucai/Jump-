/*==============================================================================

	ゲームの足場処理　[Game_Scaffold.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 27
==============================================================================*/
#include <d3dx9.h>
#include "Game_Scaffold.h"
#include "Sprite.h"
#include "Texture.h"
#include "StageSelect.h"

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// 足場出現管理構造体
typedef struct ScaffoldSpawn_tag {
	int row;					// 左上の行
	int column;					// 左上の列
	int numHorizontal;			// 横の数
	int numVertical;			// 縦の数(床はどの数字入れても1で固定)
	ScaffoldType type;			// 足場のタイプ
	GimmickColor gimmick_color;	// ギミックカラー(今のところブロックのみ、床に設定しても変化なし)
}ScaffoldSpawn;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureId_Block = TEXTURE_INVALID_ID;			// ブロック(ギミック無し)
static int g_TextureId_Block_Red = TEXTURE_INVALID_ID;		// ブロック(赤)
static int g_TextureId_Block_Green = TEXTURE_INVALID_ID;	// ブロック(緑)
static int g_TextureId_Block_Blue = TEXTURE_INVALID_ID;		// ブロック(青)
static int g_TextureId_Block_Yellow = TEXTURE_INVALID_ID;	// ブロック(黄)

static int g_TextureId_Block_Red_Off = TEXTURE_INVALID_ID;		// ブロック(赤)、オフ
static int g_TextureId_Block_Green_Off = TEXTURE_INVALID_ID;	// ブロック(緑)、オフ
static int g_TextureId_Block_Blue_Off = TEXTURE_INVALID_ID;		// ブロック(青)、オフ
static int g_TextureId_Block_Yellow_Off = TEXTURE_INVALID_ID;	// ブロック(黄)、オフ

static int g_TextureId_Floor = TEXTURE_INVALID_ID;	// 板

static Scaffold g_Scaffold[SCAFFOLD_MAX];

static int g_NowStage = 0;	// 現在のステージ

// 出現させる足場を～行～列目形式で指定（0行0列目からスタート)
// タイプはenumで指定

// チュートリアルステージ
static ScaffoldSpawn g_ScaffoldSpawnData0[] = {
{12, 0,    32, 1,    TYPE_BLOCK_ON,   GIMMICK_NONE},
{12, 31,    1, 12,    TYPE_BLOCK_ON,   GIMMICK_NONE},
{20,  5,    3,  1,  TYPE_FLOOR,     GIMMICK_NONE},
{20,  12,   3,  1,  TYPE_FLOOR,     GIMMICK_NONE},
};

// ステージ1
static ScaffoldSpawn g_ScaffoldSpawnData1[] = {
	{13,  34,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
	{14,   4,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
	{17,   8,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
	{20,   8,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ2
static ScaffoldSpawn g_ScaffoldSpawnData2[] = {
{9,   35,   8,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{13,  25,  18,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{19,  25,  18,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{11,  1,  3,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{12,  1,  3,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{13,  1,  5,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{14,  1,  5,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{15,  1,  7,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{16,  1,  7,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{17,  1,  9,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{18,  1,  9,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{19,  1, 11,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{20,  1, 11,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{21,  1, 13,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{22,  1, 15,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{23,  1, 15,   1, TYPE_BLOCK_ON,   GIMMICK_NONE},
{13, 23,  1,   6, TYPE_BLOCK_ON,   GIMMICK_NONE},
{13, 24,  1,   6, TYPE_BLOCK_ON,   GIMMICK_NONE},
{0,  33,  1,   6, TYPE_BLOCK_ON,   GIMMICK_NONE},
{0,  34,  1,   6, TYPE_BLOCK_ON,   GIMMICK_NONE},
{9,  33,  1,   4, TYPE_BLOCK_ON,   GIMMICK_NONE},
{9,  34,  1,   4, TYPE_BLOCK_ON,   GIMMICK_NONE},
{21, 41,  1,   3, TYPE_BLOCK_ON,   GIMMICK_NONE},
{21, 42,  1,   3, TYPE_BLOCK_ON,   GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ3
static ScaffoldSpawn g_ScaffoldSpawnData3[] = {
{13,  41,   2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{14,   1,   2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{18,   1,   2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{18,  41,   2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{20,   8,   4,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{20,  19,   4,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{20,  30,   4,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{22,   1,   2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{22,  41,   2,  1,  TYPE_FLOOR,  GIMMICK_NONE},

{10,   3,   1,  14,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,   4,   1,  14,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,   5,   1,  14,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,  38,   1,  14,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,  39,   1,  14,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,  40,   1,  14,  TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ4
static ScaffoldSpawn g_ScaffoldSpawnData4[] = {
{ 8,  29,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{11,  29,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{19,   9,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{4,   28,   4,  1,  TYPE_BLOCK_OFF,  GIMMICK_GREEN},
{5,   28,   4,  1,  TYPE_BLOCK_OFF,  GIMMICK_GREEN},
{15,   8,   4,  1,  TYPE_BLOCK_ON,   GIMMICK_GREEN},
{0,   0,    1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{4,   1,   27,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{4,  32,   11,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{5,   1,   27,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{5,  32,   11,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,   6,   14,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,   6,  14,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{11,   6,  1,   5,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{11,   7,  1,   5,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15,  12,  1,   9,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15,  13,  1,   9,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,   43,  1,  24,  TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ5
static ScaffoldSpawn g_ScaffoldSpawnData5[] = {
{9,  3, 7,  1,  TYPE_FLOOR,  GIMMICK_NONE},

{15,18, 6,  1,  TYPE_BLOCK_ON,  GIMMICK_RED},

{0,    0,  1,  24,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15,  24, 19,   1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15,  24, 19,   1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{16,  24, 19,   1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{17,  24, 19,   1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,   43,   1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},

};

// ステージ6
static ScaffoldSpawn g_ScaffoldSpawnData6[] = {
{19,   3,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{15,   3,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},

{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{19,  1,  2,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{19,  6, 31,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{19, 40,  3,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{4,  32,  1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{4,  33,  4,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{4,  40,  3,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,  33,  4,  1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{10,  40,  3,  1, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ7
static ScaffoldSpawn g_ScaffoldSpawnData7[] = {
{20, 12,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{14, 17,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{17, 19,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{21, 17,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{14, 22,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{17, 24,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{21, 22,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{14, 29,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{17, 27,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{20, 29,    1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{14, 34,    2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{18, 34,    2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{22, 34,    2,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{14, 40,    3,  1,  TYPE_FLOOR,  GIMMICK_NONE},

{ 0,  0,    1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 8,  1,    1,  16, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 8,  2,    1,  16, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 8,  3,    1,  16, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 8,  4,    1,  16, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 8,  5,    1,  16, TYPE_BLOCK_ON,  GIMMICK_NONE},
{18,  6,    4,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15, 10,    1,  9,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15, 11,    1,  9,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{11, 15,    1,  13, TYPE_BLOCK_ON,  GIMMICK_NONE},
{11, 16,    1,  13, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  20,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  21,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  25,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  26,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  30,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  31,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  38,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{9,  39,    1,  15, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,    1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ8
static ScaffoldSpawn g_ScaffoldSpawnData8[] = {
{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{4,   2,  5,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{20,  1, 42,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{19, 39,  6,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{18, 40,  4,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{17, 41,  2,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{16, 42,  1,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},

};

// ステージ9
static ScaffoldSpawn g_ScaffoldSpawnData9[] = {
{11,  16,   1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{11,  26,   1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{15,  16,   1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{15,  26,   1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{19,  16,   1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{19,  26,   1,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  15,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  27,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{3,  16,  3,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{3,  23,  4,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{4,  16,  4,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{5,  16,  5,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ10
static ScaffoldSpawn g_ScaffoldSpawnData10[] = {
{ 5,  3,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{ 9, 38,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{13, 38,   3,  1,  TYPE_FLOOR,  GIMMICK_NONE},

{16,  1,  42,  1,  TYPE_BLOCK_ON, GIMMICK_RED},

{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  35,  1,   9, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ11
static ScaffoldSpawn g_ScaffoldSpawnData11[] = {
{16,  3,    8,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{16, 33,    7,  1,  TYPE_FLOOR,  GIMMICK_NONE},

{20,   3,   7,  1,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{21,   3,   7,  1,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{22,   3,   7,  1,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{23,   3,   7,  1,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{6,   13,   1,  9,  TYPE_BLOCK_OFF, GIMMICK_GREEN},
{16,  13,   1,  8,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{15,  19,   5,  1,  TYPE_BLOCK_OFF, GIMMICK_GREEN},
{6,   28,   1,  9,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{16,  28,   1,  8,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{20,  33,   7,  1,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{21,  33,   1,  3,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{21,  39,   1,  3,  TYPE_BLOCK_ON,  GIMMICK_GREEN},

{0,    0,  1,  24,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{5,    1,  42,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15,  13,   6,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15,  24,   9,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{15,  40,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,   43,   1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},

};

// ステージ12
static ScaffoldSpawn g_ScaffoldSpawnData12[] = {
{21, 19,  6,  1,  TYPE_FLOOR,  GIMMICK_NONE},
{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,   4,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,   5,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  38,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  39,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{5,  17, 10,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},

};

// ステージ13
static ScaffoldSpawn g_ScaffoldSpawnData13[] = {
{6,  28,  4,  1,  TYPE_BLOCK_ON, GIMMICK_BLUE},
{7,  28,  1,  7,  TYPE_BLOCK_ON, GIMMICK_BLUE},
{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{6,  32, 11,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{14, 26, 18,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{19,  1, 42,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ14
static ScaffoldSpawn g_ScaffoldSpawnData14[] = {
{ 7, 19,  4,   1, TYPE_BLOCK_ON,  GIMMICK_RED},
{18, 15,  1,   6, TYPE_BLOCK_ON,  GIMMICK_GREEN},
{18, 16,  1,   6, TYPE_BLOCK_ON,  GIMMICK_GREEN},
{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  18,  1,   2, TYPE_BLOCK_ON,  GIMMICK_NONE},
{11,  1,  1,  13, TYPE_BLOCK_ON,  GIMMICK_NONE},
{11,  2,  1,  13, TYPE_BLOCK_ON,  GIMMICK_NONE},
{11,  3,  1,  13, TYPE_BLOCK_ON,  GIMMICK_NONE},
{11,  4,  1,  13, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 5, 15,  4,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 5, 23, 20,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 6, 15,  4,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 6, 23, 20,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 7, 15,  1,  11, TYPE_BLOCK_ON,  GIMMICK_NONE},
{ 7, 16,  1,  11, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ15
static ScaffoldSpawn g_ScaffoldSpawnData15[] = {
	{ 0,  22,   1,  9,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{ 0,  38,   1,  4,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{ 8,   5,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{ 8,  27,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{ 9,  14,   1,  3,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{ 9,  38,   1,  8,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{14,  20,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{18,   0,   6,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{18,   5,   1,  6,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{18,  32,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{20,  15,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
	{20,  39,   3,  1,  TYPE_FLOOR,     GIMMICK_NONE},

};

// ステージ16
static ScaffoldSpawn g_ScaffoldSpawnData16[] = {
{10,  0,   16,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  24,    2,  10,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{14,  24,   2,  10,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{16,  26,   2,  8,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{18,  28,   2,  6,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{21,  31,   2,  3,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{18,  12, 2, 6, TYPE_BLOCK_ON, GIMMICK_BLUE},
{21,  14, 2, 3, TYPE_BLOCK_ON, GIMMICK_BLUE},
{10,  23, 1, 4, TYPE_BLOCK_ON, GIMMICK_BLUE},
{13,  24, 1, 1, TYPE_BLOCK_ON, GIMMICK_BLUE},
{9,  40, 4, 1, TYPE_BLOCK_ON, GIMMICK_BLUE},
{14,  21, 3, 1, TYPE_FLOOR, GIMMICK_NONE},
{20,  36, 1, 4, TYPE_BLOCK_ON, GIMMICK_BLUE},
{20,  39, 1, 4, TYPE_BLOCK_ON, GIMMICK_BLUE},
{20,  40, 6, 1, TYPE_BLOCK_ON, GIMMICK_BLUE},
};

// ステージ17
static ScaffoldSpawn g_ScaffoldSpawnData17[] = {
{0,   0,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  43,  1,  24, TYPE_BLOCK_ON,  GIMMICK_NONE},
{18,  1, 42,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{6,   9,  1,   5, TYPE_BLOCK_ON,  GIMMICK_NONE},
{8,   8,  1,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{8,  10,  1,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{6,  36,  1,   5, TYPE_BLOCK_ON,  GIMMICK_NONE},
{8,  35,  1,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
{8,  37,  1,   1, TYPE_BLOCK_ON,  GIMMICK_NONE},
};

// ステージ18
static ScaffoldSpawn g_ScaffoldSpawnData18[] = {
{5,  0,   8,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{5,  11,   9,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{6,  16,   4,  2,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{5,  25,   12,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{6,  25,   4,  2,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{16,  42,   2,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{22,  0,    2,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{22,  42,   2,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{23,  0,    3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{23,  41,   3,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{19,  3,   5,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{19,  11,   9,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{19,  25,   17,  1,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{16,  25,   16,  8,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{13,  18,   2,  6,  TYPE_BLOCK_ON,  GIMMICK_NONE},
{0,  8,   3, 4,  TYPE_BLOCK_ON, GIMMICK_RED},
{0,  20,  5, 4,  TYPE_BLOCK_ON, GIMMICK_RED},
{7,  20,  5, 5,  TYPE_BLOCK_ON, GIMMICK_RED},
{15,  20, 5, 6,  TYPE_BLOCK_ON, GIMMICK_RED},
{21,  20, 1, 2,  TYPE_BLOCK_ON, GIMMICK_RED},
{21,  24, 1, 2,  TYPE_BLOCK_ON, GIMMICK_RED},
{6,  37, 6, 1, TYPE_FLOOR, GIMMICK_NONE},

};

// ステージ19
static ScaffoldSpawn g_ScaffoldSpawnData19[] = {
{13, 3, 3, 2,        TYPE_BLOCK_ON,  GIMMICK_RED},
{6, 37, 3, 2,        TYPE_BLOCK_ON,  GIMMICK_RED},
{ 12,  10,   3,  2,  TYPE_BLOCK_ON,  GIMMICK_BLUE},
{ 11,  30,   3,  2,  TYPE_BLOCK_ON,  GIMMICK_BLUE},
{ 13,  15,   5,  1,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{ 13,  23,   5,  1,  TYPE_BLOCK_ON,  GIMMICK_GREEN},
{14,  20,   3,  1,  TYPE_FLOOR,     GIMMICK_NONE},
};

// ステージ20
static ScaffoldSpawn g_ScaffoldSpawnData20[] = {
//{1,  14,  TIGHTNESS_MAX  *  1.0f,   GIMMICK_YELLOW},
// {11,  32,  TIGHTNESS_MAX  *  1.0f,  GIMMICK_BLUE},
// {3,   4,  TIGHTNESS_MAX  *  1.0f,   GIMMICK_RED},
// {8,   7,  TIGHTNESS_MAX  *  0.8f,   GIMMICK_RED},
// {15,  9,  TIGHTNESS_MAX  *  0.8f,   GIMMICK_GREEN},
// {16,  28,  TIGHTNESS_MAX  *  0.8f,  GIMMICK_GREEN},
// { 5,  38,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE},
// { 8,  40,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE},
// {11,  38,  TIGHTNESS_MAX  * -0.3f,  GIMMICK_NONE},
// {14,  40,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE},
// {13,  21,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE},
// {16,  18,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE},
// {17,  38,  TIGHTNESS_MAX  *  -0.3f, GIMMICK_NONE},
// {18,  14,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE},
	{7,  22, 3, 1, TYPE_BLOCK_ON, GIMMICK_BLUE},
	{13, 25, 3, 1, TYPE_BLOCK_ON, GIMMICK_RED},
	{7,  28, 1, 7, TYPE_BLOCK_ON, GIMMICK_GREEN},
	{0,  35, 1, 6, TYPE_BLOCK_ON, GIMMICK_YELLOW},
	{20, 36, 1, 4, TYPE_BLOCK_ON, GIMMICK_YELLOW},
	{20, 42, 1, 4, TYPE_BLOCK_ON, GIMMICK_YELLOW},
	{20, 37, 5, 1, TYPE_BLOCK_ON, GIMMICK_YELLOW},
	{16, 35, 1, 8, TYPE_BLOCK_ON, GIMMICK_GREEN},
	{6,   28, 8, 1, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{7,   35, 1, 9, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{10,   2, 4, 1, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{11,   2, 4, 1, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{14,  24, 1, 10, TYPE_BLOCK_ON, GIMMICK_NONE},
	{17,  32, 3, 1, TYPE_FLOOR,     GIMMICK_NONE},
	{20,   2, 1, 4, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{20,   3, 1, 4, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{20,  19, 1, 4, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{20,  20, 1, 4, TYPE_BLOCK_ON,  GIMMICK_NONE},
	{21,  32, 3, 1, TYPE_FLOOR,     GIMMICK_NONE},
	{12,  11, 3, 1, TYPE_FLOOR,     GIMMICK_NONE},
};

// ステージ番号で各ステージのScaffoldSpawnDataを読み込む構造体配列ポインタ配列
static ScaffoldSpawn* g_pScaffoldSpawn[] = {
	g_ScaffoldSpawnData0,
	g_ScaffoldSpawnData1,
	g_ScaffoldSpawnData2,
	g_ScaffoldSpawnData3,
	g_ScaffoldSpawnData4,
	g_ScaffoldSpawnData5,
	g_ScaffoldSpawnData6,
	g_ScaffoldSpawnData7,
	g_ScaffoldSpawnData8,
	g_ScaffoldSpawnData9,
	g_ScaffoldSpawnData10,
	g_ScaffoldSpawnData11,
	g_ScaffoldSpawnData12,
	g_ScaffoldSpawnData13,
	g_ScaffoldSpawnData14,
	g_ScaffoldSpawnData15,
	g_ScaffoldSpawnData16,
	g_ScaffoldSpawnData17,
	g_ScaffoldSpawnData18,
	g_ScaffoldSpawnData19,
	g_ScaffoldSpawnData20,
};

// 各ステージのScaffoldSpawnDataを読み込んで格納するポインタ
static ScaffoldSpawn* g_pScaffoldSpawnData = NULL;

// 各ステージの足場の数
// それぞれ配列の要素数を数えてる
static size_t g_numScaffold[] = {
	sizeof(g_ScaffoldSpawnData0) / sizeof(g_ScaffoldSpawnData0[0]),
	sizeof(g_ScaffoldSpawnData1) / sizeof(g_ScaffoldSpawnData1[0]),
	sizeof(g_ScaffoldSpawnData2) / sizeof(g_ScaffoldSpawnData2[0]),
	sizeof(g_ScaffoldSpawnData3) / sizeof(g_ScaffoldSpawnData3[0]),
	sizeof(g_ScaffoldSpawnData4) / sizeof(g_ScaffoldSpawnData4[0]),
	sizeof(g_ScaffoldSpawnData5) / sizeof(g_ScaffoldSpawnData5[0]),
	sizeof(g_ScaffoldSpawnData6) / sizeof(g_ScaffoldSpawnData6[0]),
	sizeof(g_ScaffoldSpawnData7) / sizeof(g_ScaffoldSpawnData7[0]),
	sizeof(g_ScaffoldSpawnData8) / sizeof(g_ScaffoldSpawnData8[0]),
	sizeof(g_ScaffoldSpawnData9) / sizeof(g_ScaffoldSpawnData9[0]),
	sizeof(g_ScaffoldSpawnData10) / sizeof(g_ScaffoldSpawnData10[0]),
	sizeof(g_ScaffoldSpawnData11) / sizeof(g_ScaffoldSpawnData11[0]),
	sizeof(g_ScaffoldSpawnData12) / sizeof(g_ScaffoldSpawnData12[0]),
	sizeof(g_ScaffoldSpawnData13) / sizeof(g_ScaffoldSpawnData13[0]),
	sizeof(g_ScaffoldSpawnData14) / sizeof(g_ScaffoldSpawnData14[0]),
	sizeof(g_ScaffoldSpawnData15) / sizeof(g_ScaffoldSpawnData15[0]),
	sizeof(g_ScaffoldSpawnData16) / sizeof(g_ScaffoldSpawnData16[0]),
	sizeof(g_ScaffoldSpawnData17) / sizeof(g_ScaffoldSpawnData17[0]),
	sizeof(g_ScaffoldSpawnData18) / sizeof(g_ScaffoldSpawnData18[0]),
	sizeof(g_ScaffoldSpawnData19) / sizeof(g_ScaffoldSpawnData19[0]),
	sizeof(g_ScaffoldSpawnData20) / sizeof(g_ScaffoldSpawnData20[0]),
};

// 現在のステージの足場の数
static size_t g_ScaffoldSpawnDataCount = 0;

void GameScaffold_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureId_Block = Texture_SetTextureLoadFile("asset/texture/blockblack02.png");
	g_TextureId_Block_Red = Texture_SetTextureLoadFile("asset/texture/blockred02.png");
	g_TextureId_Block_Green = Texture_SetTextureLoadFile("asset/texture/blockgreen02.png");
	g_TextureId_Block_Blue = Texture_SetTextureLoadFile("asset/texture/blockblue03.png");
	g_TextureId_Block_Yellow = Texture_SetTextureLoadFile("asset/texture/blockyellow02.png");

	g_TextureId_Block_Red_Off = Texture_SetTextureLoadFile("asset/texture/block_red_off.png");
	g_TextureId_Block_Green_Off = Texture_SetTextureLoadFile("asset/texture/block_green_off.png");
	g_TextureId_Block_Blue_Off = Texture_SetTextureLoadFile("asset/texture/block_blue_off.png");
	g_TextureId_Block_Yellow_Off = Texture_SetTextureLoadFile("asset/texture/block_yellow_off.png");

	g_TextureId_Floor = Texture_SetTextureLoadFile("asset/texture/floor.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}

	// ステージ番号をセット
	g_NowStage = StageSelect_GetStageNumber();

	GameScaffold_Reset();
}

void GameScaffold_Finalize(void)
{
	Texture_Release(&g_TextureId_Block, 1);
	Texture_Release(&g_TextureId_Block_Red, 1);
	Texture_Release(&g_TextureId_Block_Green, 1);
	Texture_Release(&g_TextureId_Block_Blue, 1);
	Texture_Release(&g_TextureId_Block_Yellow, 1);

	Texture_Release(&g_TextureId_Block_Red_Off, 1);
	Texture_Release(&g_TextureId_Block_Green_Off, 1);
	Texture_Release(&g_TextureId_Block_Blue_Off, 1);
	Texture_Release(&g_TextureId_Block_Yellow_Off, 1);

	Texture_Release(&g_TextureId_Floor, 1);
}
void GameScaffold_Update(void)
{

}

void GameScaffold_Draw(void)
{
	for (int i = 0; i < g_ScaffoldSpawnDataCount; i++) {
		switch (g_Scaffold[i].type)
		{
		case TYPE_NONE:
			break;
		case TYPE_BLOCK_ON:
			for (int j = 0; j < g_Scaffold[i].numVertical; j++) {
				for (int k = 0; k < g_Scaffold[i].numHorizontal; k++) {
					Sprite_Draw(g_Scaffold[i].texture_id, g_Scaffold[i].position.x + k * SCAFFOLD_SIZE,
						g_Scaffold[i].position.y + j * SCAFFOLD_SIZE,
						SCAFFOLD_SIZE, SCAFFOLD_SIZE, 0, 0, 590, 590);
				}
			}
			break;
		case TYPE_BLOCK_OFF:
			for (int j = 0; j < g_Scaffold[i].numVertical; j++) {
				for (int k = 0; k < g_Scaffold[i].numHorizontal; k++) {
					Sprite_Draw(g_Scaffold[i].texture_id_off, g_Scaffold[i].position.x + k * SCAFFOLD_SIZE,
						g_Scaffold[i].position.y + j * SCAFFOLD_SIZE, SCAFFOLD_SIZE, SCAFFOLD_SIZE, 0, 0, 131, 131);
				}
			}
			break;
		case TYPE_FLOOR:
			Sprite_Draw(g_TextureId_Floor, g_Scaffold[i].position.x, g_Scaffold[i].position.y,
				g_Scaffold[i].width, FLOOR_HEIGHT, 0, 0, 591, 118);
			break;
		}
	}
}

Scaffold* GameScaffold_GetScaffold(int i)
{
	return &g_Scaffold[i];
}

size_t GameScaffold_GetCount(void)
{
	return g_ScaffoldSpawnDataCount;
}

void GameScaffold_Switch(GimmickColor gimmickColor)
{
	if (gimmickColor != GIMMICK_GREEN && gimmickColor != GIMMICK_YELLOW) {
		return;
	}

	for (int i = 0; i < g_ScaffoldSpawnDataCount; i++) {
		// 指定した（ネジと同じ）ギミックカラーなら切り替え
		// また、切り替えるのはギミックカラーが緑色or黄色の時のみ
		if (gimmickColor != g_Scaffold[i].gimmick_color) {
			continue;
		}
		// オンだったらオフ、オフだったらオンにする
		switch (g_Scaffold[i].type)
		{
		case TYPE_BLOCK_ON:
			g_Scaffold[i].type = TYPE_BLOCK_OFF;
			break;
		case TYPE_BLOCK_OFF:
			g_Scaffold[i].type = TYPE_BLOCK_ON;
			break;
		default:
			break;
		}
	}
}

// 同じギミックカラーScrewのtightnessが変動した時にtightnessに応じてブロックの座標を変動させる
void GameScaffold_Move(GimmickColor gimmickColor, float tightness)
{
	for (int i = 0; i < g_ScaffoldSpawnDataCount; i++) {
		// 指定した（ネジと同じ）ギミックカラーなら動かす
		if (gimmickColor != g_Scaffold[i].gimmick_color) {
			continue;
		}
		switch (g_Scaffold[i].gimmick_color)
		{
		case GIMMICK_RED:
			g_Scaffold[i].position.y = g_Scaffold[i].initial_position.y + SCAFFOLD_SIZE * 2 * (tightness / TIGHTNESS_MAX);
			break;
		case GIMMICK_BLUE:
			g_Scaffold[i].position.x = g_Scaffold[i].initial_position.x - SCAFFOLD_SIZE * 2 * (tightness / TIGHTNESS_MAX);
			break;
		default:
			break;
		}
	}
}

void GameScaffold_Reset(void)
{
	// 全てを未使用状態に初期化しておく
	for (int i = 0; i < SCAFFOLD_MAX; i++) {
		g_Scaffold[i].type = TYPE_NONE;
	}

	// ステージ番号に合わせてScaffoldSpawnDataをセット
	g_pScaffoldSpawnData = g_pScaffoldSpawn[g_NowStage];

	g_ScaffoldSpawnDataCount = g_numScaffold[g_NowStage];

	// g_Scaffoldをg_ScaffoldSpawnDataで設定した個数分有効にし、それぞれ指定した値を入れていく
	for (int i = 0; i < g_ScaffoldSpawnDataCount; i++) {
		g_Scaffold[i].position = D3DXVECTOR2(START_X + SCAFFOLD_SIZE * g_pScaffoldSpawnData[i].column,
			SCAFFOLD_SIZE * g_pScaffoldSpawnData[i].row);
		g_Scaffold[i].initial_position = D3DXVECTOR2(START_X + SCAFFOLD_SIZE * g_pScaffoldSpawnData[i].column,
			SCAFFOLD_SIZE * g_pScaffoldSpawnData[i].row);
		g_Scaffold[i].width = g_pScaffoldSpawnData[i].numHorizontal * SCAFFOLD_SIZE;
		g_Scaffold[i].height = g_pScaffoldSpawnData[i].numVertical * SCAFFOLD_SIZE;
		g_Scaffold[i].numHorizontal = g_pScaffoldSpawnData[i].numHorizontal;
		g_Scaffold[i].numVertical = g_pScaffoldSpawnData[i].numVertical;
		g_Scaffold[i].type = g_pScaffoldSpawnData[i].type;
		g_Scaffold[i].gimmick_color = g_pScaffoldSpawnData[i].gimmick_color;
		switch (g_Scaffold[i].gimmick_color)
		{
		case GIMMICK_NONE:
			g_Scaffold[i].texture_id = g_TextureId_Block;
			break;
		case GIMMICK_RED:
			g_Scaffold[i].texture_id = g_TextureId_Block_Red;
			g_Scaffold[i].texture_id_off = g_TextureId_Block_Red_Off;
			break;
		case GIMMICK_GREEN:
			g_Scaffold[i].texture_id = g_TextureId_Block_Green;
			g_Scaffold[i].texture_id_off = g_TextureId_Block_Green_Off;
			break;
		case GIMMICK_BLUE:
			g_Scaffold[i].texture_id = g_TextureId_Block_Blue;
			g_Scaffold[i].texture_id_off = g_TextureId_Block_Blue_Off;
			break;
		case GIMMICK_YELLOW:
			g_Scaffold[i].texture_id = g_TextureId_Block_Yellow;
			g_Scaffold[i].texture_id_off = g_TextureId_Block_Yellow_Off;
			break;
		}
	}
}