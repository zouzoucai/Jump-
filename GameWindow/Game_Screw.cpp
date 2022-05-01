/*==============================================================================

	ゲームのネジ関係の処理　[Game_Screw.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 25
==============================================================================*/
#include <d3dx9.h>

#include "Sprite.h"
#include "Texture.h"
#include "Game_Player.h"
#include "Game_Goal.h"
#include "Game_Scaffold.h"
#include "StageSelect.h"
#include "sound.h"
#include "Game_Screw.h"

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// ネジ出現管理構造体
typedef struct ScrewSpawn_tag {
	int row;						// 行
	int column;						// 列
	float tightness;				// 締まり具合（手動で設定）
	GimmickColor gimmick_color;		// ギミックカラー
	ScrewState state;				// 状態
}ScrewSpawn;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureId_Screw = TEXTURE_INVALID_ID;			// ネジ(ギミック無し)
static int g_TextureId_Screw_Good = TEXTURE_INVALID_ID;		// ネジ(ギミック無し、イイ感じ状態)
static int g_TextureId_Screw_Red = TEXTURE_INVALID_ID;		// ネジ(赤)
static int g_TextureId_Screw_Green = TEXTURE_INVALID_ID;	// ネジ(緑)
static int g_TextureId_Screw_Blue = TEXTURE_INVALID_ID;		// ネジ(青)
static int g_TextureId_Screw_Yellow = TEXTURE_INVALID_ID;	// ネジ(黄)

static int g_TextureId_Gauge1 = TEXTURE_INVALID_ID;	// ゲージの外側
static int g_TextureId_Gauge2 = TEXTURE_INVALID_ID;	// ゲージの内側
static int g_TextureId_Gauge3 = TEXTURE_INVALID_ID;	// ゲージのイイ感じポイント

static int g_TextureId_Lamp_Off = TEXTURE_INVALID_ID;	// スイッチネジの連動を知らせるランプ
static int g_TextureId_Lamp_On = TEXTURE_INVALID_ID;	// スイッチネジの連動を知らせるランプ

static Screw g_Screw[SCREW_MAX];

static int g_NowStage = 0;	// 現在のステージ

// 出現させるネジを～行～列目形式で指定（0行0列目からスタート)
// 締まり具合はTIGHTNESS_MAX *（-1.0f～1.0f）で指定

// チュートリアルステージ
static ScrewSpawn g_ScrewSpawnData0[] = {
{ 15,  6,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 15,  13,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 16,  21,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ1
static ScrewSpawn g_ScrewSpawnData1[] = {
	{ 6,  28,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
	{ 8,   9,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
	{ 8,  22,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
	{10,  15,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ2
static ScrewSpawn g_ScrewSpawnData2[] = {
{ 7,  11,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 7,  19,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  28,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 10, 39,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 16, 31,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 16, 39,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ3
static ScrewSpawn g_ScrewSpawnData3[] = {
{ 10,  8,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
{ 10, 33,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
{ 11, 17,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
{ 12, 26,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
};

// ステージ4
static ScrewSpawn g_ScrewSpawnData4[] = {
{ 19,   17,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_GREEN, STATE_SPRING_OFF},
{ 21,   6,   TIGHTNESS_MAX  *  0.5f,  GIMMICK_GREEN, STATE_SPRING_OFF},
{ 13,   10,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_GREEN, STATE_SWITCH_OFF},
{ 14,   30,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 15,   23,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ5
static ScrewSpawn g_ScrewSpawnData5[] = {
{ 18,   12,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED, STATE_SPRING_OFF},

{  8,   13,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{  6,   18,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{  9,   23,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ6
static ScrewSpawn g_ScrewSpawnData6[] = {
{ 9,   5,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 7,  10,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 5,  15,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 5,  21,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  24,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 5,  28,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ7
static ScrewSpawn g_ScrewSpawnData7[] = {
{  2,   26,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{  4,   21,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{  4,   31,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{  5,   39,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{  7,   16,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{  7,   35,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 10,   11,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ8
static ScrewSpawn g_ScrewSpawnData8[] = {
{ 6,  10,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 9,  17,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{10,  24,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{10,  30,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  36,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{11,  41,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ9
static ScrewSpawn g_ScrewSpawnData9[] = {
{ 6,  23,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE, STATE_BAD},
{ 9,  19,  TIGHTNESS_MAX  * -0.3f,  GIMMICK_NONE, STATE_BAD},
{12,  23,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE, STATE_BAD},
{16,  19,  TIGHTNESS_MAX  * -0.3f,  GIMMICK_NONE, STATE_BAD},
{18,  23,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ10
static ScrewSpawn g_ScrewSpawnData10[] = {
{4,   39,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED, STATE_SPRING_OFF},

{ 3,   9,  TIGHTNESS_MAX  *  -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 5,  17,  TIGHTNESS_MAX  *  -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 6,  24,  TIGHTNESS_MAX  *  -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  32,  TIGHTNESS_MAX  *  -0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ11
static ScrewSpawn g_ScrewSpawnData11[] = {
{ 10,   6,   TIGHTNESS_MAX  *  0.5f,  GIMMICK_GREEN, STATE_SPRING_OFF},
{ 10,  36,   TIGHTNESS_MAX  *  0.5f,  GIMMICK_GREEN, STATE_SPRING_OFF},

{ 10,   21,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 17,   21,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 19,   17,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 19,   25,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},


};

// ステージ12
static ScrewSpawn g_ScrewSpawnData12[] = {
{15,  17,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{14,  21,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{14,  26,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{10,  26,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  22,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  17,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  11,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 4,  11,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ13
static ScrewSpawn g_ScrewSpawnData13[] = {
{13,   8,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE, STATE_SPRING_OFF},
{ 8,   8,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE, STATE_SWITCH_OFF},
{ 4,   8,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 6,  13,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 4,  18,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 4,  23,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ14
static ScrewSpawn g_ScrewSpawnData14[] = {
{  3,   13,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_GREEN, STATE_SPRING_OFF},
{ 10,   26,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED,   STATE_SPRING_OFF},
{ 7,  10,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{10,   8,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{13,   6,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{19,  12,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{20,  18,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{19,  23,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{17,  27,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{14,  31,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},

};

// ステージ15
static ScrewSpawn g_ScrewSpawnData15[] = {
	{ 3,  28,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
	{ 5,  10,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
	{ 6,  16,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
	{ 6,  20,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
	{ 6,  32,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
	{ 7,  24,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
	{11,  25,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE,STATE_BAD},
	{14,  16,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
	{15,  10,  TIGHTNESS_MAX  *  0.1f,  GIMMICK_NONE,STATE_BAD},
	{15,  29,  TIGHTNESS_MAX  * -0.4f,  GIMMICK_NONE,STATE_BAD},
	{17,  25,  TIGHTNESS_MAX  *  0.4f,  GIMMICK_NONE,STATE_BAD},
	{19,  21,  TIGHTNESS_MAX  *  0.4f,  GIMMICK_NONE,STATE_BAD},
};

// ステージ16
static ScrewSpawn g_ScrewSpawnData16[] = {
{ 19,  5,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 15,  7,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 14,  12, TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE, STATE_SPRING_OFF},
{ 10,  19, TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 6,  19, TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE, STATE_SPRING_OFF},
{ 11,  28, TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE, STATE_SPRING_OFF},
{ 11,  32, TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE, STATE_SWITCH_OFF},
{ 2,  30,  TIGHTNESS_MAX  *  -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 2,  35,  TIGHTNESS_MAX  *  -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 2,  40,  TIGHTNESS_MAX  *  -0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ17
static ScrewSpawn g_ScrewSpawnData17[] = {
{ 3,   9,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,   5,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  13,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{13,   9,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  20,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  26,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 3,  36,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  32,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  40,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{13,  36,  TIGHTNESS_MAX  * -0.5f,  GIMMICK_NONE, STATE_BAD},
};

// ステージ18
static ScrewSpawn g_ScrewSpawnData18[] = {
{ 1,  5, TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED, STATE_SPRING_OFF},
{ 15,  6, TIGHTNESS_MAX  *    0.5f,  GIMMICK_RED, STATE_SPRING_OFF},
{ 11,  11,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE, STATE_BAD},
{ 11,  16,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE, STATE_BAD},
{ 10,  28,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 8,  32,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 10,  36,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 7,  40,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE, STATE_BAD},
{ 2,  32, TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED, STATE_SPRING_OFF},
{ 2,  28, TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED, STATE_SWITCH_OFF},
};

// ステージ19
static ScrewSpawn g_ScrewSpawnData19[] = {
{ 5,  4,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,   STATE_BAD},
{ 3,  11,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,  STATE_BAD},
{ 3,  31,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,  STATE_BAD},
{ 1,  28,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,  STATE_BAD},
{ 17,  21, TIGHTNESS_MAX  *  0.5f,  GIMMICK_GREEN, STATE_SPRING_OFF},
{ 17,  11, TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED,   STATE_SPRING_OFF},
{ 17,  31, TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE,  STATE_SPRING_OFF},
{ 17,  16, TIGHTNESS_MAX  *  0.5f,  GIMMICK_RED,   STATE_SWITCH_OFF},
{ 17,  26, TIGHTNESS_MAX  *  0.5f,  GIMMICK_BLUE,  STATE_SWITCH_OFF},
};

// ステージ20
static ScrewSpawn g_ScrewSpawnData20[] = {
{1,  14,  TIGHTNESS_MAX  *  1.0f,   GIMMICK_YELLOW,STATE_BAD},
 {11,  32,  TIGHTNESS_MAX  *  1.0f,  GIMMICK_BLUE,STATE_SPRING_OFF},
 {3,   4,  TIGHTNESS_MAX  *  1.0f,   GIMMICK_RED,STATE_SPRING_OFF},
 {8,   7,  TIGHTNESS_MAX  *  0.8f,   GIMMICK_RED,STATE_SPRING_OFF},
 {15,  9,  TIGHTNESS_MAX  *  0.8f,   GIMMICK_GREEN,STATE_SPRING_OFF},
 {16,  28,  TIGHTNESS_MAX  *  0.8f,  GIMMICK_GREEN,STATE_SWITCH_OFF},
 { 5,  38,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE,STATE_BAD},
 { 8,  40,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE,STATE_BAD},
 {11,  38,  TIGHTNESS_MAX  * -0.3f,  GIMMICK_NONE,STATE_BAD},
 {14,  40,  TIGHTNESS_MAX  *  0.3f,  GIMMICK_NONE,STATE_BAD},
 {13,  21,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
 {16,  18,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
 {17,  38,  TIGHTNESS_MAX  *  -0.3f, GIMMICK_NONE,STATE_BAD},
 {18,  14,  TIGHTNESS_MAX  *  0.5f,  GIMMICK_NONE,STATE_BAD},
};

// ステージ番号で各ステージのScrewSpawnDataを読み込む構造体配列ポインタ配列
static ScrewSpawn* g_pScrewSpawn[] = {
	g_ScrewSpawnData0,
	g_ScrewSpawnData1,
	g_ScrewSpawnData2,
	g_ScrewSpawnData3,
	g_ScrewSpawnData4,
	g_ScrewSpawnData5,
	g_ScrewSpawnData6,
	g_ScrewSpawnData7,
	g_ScrewSpawnData8,
	g_ScrewSpawnData9,
	g_ScrewSpawnData10,
	g_ScrewSpawnData11,
	g_ScrewSpawnData12,
	g_ScrewSpawnData13,
	g_ScrewSpawnData14,
	g_ScrewSpawnData15,
	g_ScrewSpawnData16,
	g_ScrewSpawnData17,
	g_ScrewSpawnData18,
	g_ScrewSpawnData19,
	g_ScrewSpawnData20,
};

// 各ステージのScrewSpawnDataを読み込んで格納するポインタ
static ScrewSpawn* g_pScrewSpawnData = NULL;

// 各ステージのネジの数
// それぞれ配列の要素数を数えてる
static size_t g_numScrew[] = {
	sizeof(g_ScrewSpawnData0) / sizeof(g_ScrewSpawnData0[0]),
	sizeof(g_ScrewSpawnData1) / sizeof(g_ScrewSpawnData1[0]),
	sizeof(g_ScrewSpawnData2) / sizeof(g_ScrewSpawnData2[0]),
	sizeof(g_ScrewSpawnData3) / sizeof(g_ScrewSpawnData3[0]),
	sizeof(g_ScrewSpawnData4) / sizeof(g_ScrewSpawnData4[0]),
	sizeof(g_ScrewSpawnData5) / sizeof(g_ScrewSpawnData5[0]),
	sizeof(g_ScrewSpawnData6) / sizeof(g_ScrewSpawnData6[0]),
	sizeof(g_ScrewSpawnData7) / sizeof(g_ScrewSpawnData7[0]),
	sizeof(g_ScrewSpawnData8) / sizeof(g_ScrewSpawnData8[0]),
	sizeof(g_ScrewSpawnData9) / sizeof(g_ScrewSpawnData9[0]),
	sizeof(g_ScrewSpawnData10) / sizeof(g_ScrewSpawnData10[0]),
	sizeof(g_ScrewSpawnData11) / sizeof(g_ScrewSpawnData11[0]),
	sizeof(g_ScrewSpawnData12) / sizeof(g_ScrewSpawnData12[0]),
	sizeof(g_ScrewSpawnData13) / sizeof(g_ScrewSpawnData13[0]),
	sizeof(g_ScrewSpawnData14) / sizeof(g_ScrewSpawnData14[0]),
	sizeof(g_ScrewSpawnData15) / sizeof(g_ScrewSpawnData15[0]),
	sizeof(g_ScrewSpawnData16) / sizeof(g_ScrewSpawnData16[0]),
	sizeof(g_ScrewSpawnData17) / sizeof(g_ScrewSpawnData17[0]),
	sizeof(g_ScrewSpawnData18) / sizeof(g_ScrewSpawnData18[0]),
	sizeof(g_ScrewSpawnData19) / sizeof(g_ScrewSpawnData19[0]),
	sizeof(g_ScrewSpawnData20) / sizeof(g_ScrewSpawnData20[0]),
};

// 現在のステージのネジの数
static size_t g_ScrewSpawnDataCount = 0;

// クリア条件に関わるネジの数
static int g_ScrewClearCount = 0;

// イイ感じのねじの数
static int g_GoodCount = 0;

// 各ギミックカラー毎のSTATE_SPRING_OFFでないぜんまいねじの数
static int g_RedOnNumber = 0;
static int g_GreenOnNumber = 0;
static int g_BlueOnNumber = 0;
static int g_YellowOnNumber = 0;

void GameScrew_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureId_Screw = Texture_SetTextureLoadFile("asset/texture/screw.png");
	g_TextureId_Screw_Good = Texture_SetTextureLoadFile("asset/texture/screw_good.png");
	g_TextureId_Screw_Red = Texture_SetTextureLoadFile("asset/texture/screw_red改.png");
	g_TextureId_Screw_Green = Texture_SetTextureLoadFile("asset/texture/screw_green改.png");
	g_TextureId_Screw_Blue = Texture_SetTextureLoadFile("asset/texture/screw_blue改.png");
	g_TextureId_Screw_Yellow = Texture_SetTextureLoadFile("asset/texture/screw_yellow改.png");


	g_TextureId_Gauge1 = Texture_SetTextureLoadFile("asset/texture/gauge_outside.png");
	g_TextureId_Gauge2 = Texture_SetTextureLoadFile("asset/texture/gauge_inside.png");
	g_TextureId_Gauge3 = Texture_SetTextureLoadFile("asset/texture/gauge_point.png");

	g_TextureId_Lamp_Off = Texture_SetTextureLoadFile("asset/texture/lamp_off.png");
	g_TextureId_Lamp_On = Texture_SetTextureLoadFile("asset/texture/lamp_on.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}

	// ステージ番号をセット
	g_NowStage = StageSelect_GetStageNumber();

	GameScrew_Reset();
}

void GameScrew_Finalize(void)
{
	Texture_Release(&g_TextureId_Screw, 1);
	Texture_Release(&g_TextureId_Screw_Good, 1);
	Texture_Release(&g_TextureId_Screw_Red, 1);
	Texture_Release(&g_TextureId_Screw_Green, 1);
	Texture_Release(&g_TextureId_Screw_Blue, 1);
	Texture_Release(&g_TextureId_Screw_Yellow, 1);

	Texture_Release(&g_TextureId_Gauge1, 1);
	Texture_Release(&g_TextureId_Gauge2, 1);
	Texture_Release(&g_TextureId_Gauge3, 1);

	Texture_Release(&g_TextureId_Lamp_Off, 1);
	Texture_Release(&g_TextureId_Lamp_On, 1);
}

void GameScrew_Update(void)
{
	for (int i = 0; i < g_ScrewSpawnDataCount; i++) {
		switch (g_Screw[i].state)
		{
		case STATE_NONE:
			break;

		case STATE_BAD:
			g_Screw[i].gauge_left_width = GAUGE_INSUDE_TOTAL * (TIGHTNESS_MAX - g_Screw[i].tightness) / (TIGHTNESS_MAX * 2);
			if (fabsf(g_Screw[i].tightness) < TIGHTNESS_GOOD_RANGE) {
				g_Screw[i].state = STATE_GOOD;
				g_Screw[i].texture_id = g_TextureId_Screw_Good;
				g_GoodCount++;
				PlaySound(SOUND_LABEL_SE_SCREW_GOOD);
				if (g_GoodCount == g_ScrewClearCount) {
					// クリア可能に
					GameGoal_ToEnable();
					PlaySound(SOUND_LABEL_SE_DOOR_OPEN);
				}
				// 現在クリア条件に関わるネジとギミックネジを分けているためここでこの処理は要らない
				//if (g_Screw[i].gimmick_color != GIMMICK_NONE && g_Screw[i].gimmick_color != GIMMICK_YELLOW) {
				//	// 同じ色のギミックのスイッチを切り替える
				//	GameScaffold_Switch(g_Screw[i].gimmick_color);
				//}
			}
			break;

		case STATE_GOOD:
			g_Screw[i].gauge_left_width = GAUGE_INSUDE_TOTAL * (TIGHTNESS_MAX - g_Screw[i].tightness) / (TIGHTNESS_MAX * 2);
			if (fabsf(g_Screw[i].tightness) > TIGHTNESS_GOOD_RANGE) {
				g_Screw[i].state = STATE_BAD;
				g_Screw[i].texture_id = g_TextureId_Screw;
				g_GoodCount--;
				if (g_GoodCount == g_ScrewClearCount - 1) {
					// クリア不可能に
					GameGoal_ToDisenable();
				}
				// 現在クリア条件に関わるネジとギミックネジを分けているためここでこの処理は要らない
				//if (g_Screw[i].gimmick_color != GIMMICK_NONE && g_Screw[i].gimmick_color != GIMMICK_YELLOW) {
				//	// 同じ色のギミックのスイッチを切り替える
				//	GameScaffold_Switch(g_Screw[i].gimmick_color);
				//}
				break;
			}
			break;

		case STATE_SPRING_ON:
			g_Screw[i].gauge_left_width = GAUGE_INSUDE_TOTAL * (TIGHTNESS_MAX - g_Screw[i].tightness) / (TIGHTNESS_MAX * 2);
			GameScrew_AutoTurned(i, TIGHTNESS_DECREASE);
			break;
		case STATE_SPRING_WORK:
			g_Screw[i].gauge_left_width = GAUGE_INSUDE_TOTAL * (TIGHTNESS_MAX - g_Screw[i].tightness) / (TIGHTNESS_MAX * 2);
			break;
		case STATE_SPRING_OFF:
			g_Screw[i].gauge_left_width = GAUGE_INSUDE_TOTAL * (TIGHTNESS_MAX - g_Screw[i].tightness) / (TIGHTNESS_MAX * 2);
			break;
		case STATE_SWITCH_ON:
			GameScrew_Switch(i);
			break;
		case STATE_SWITCH_OFF:
			GameScrew_Switch(i);
			break;
		}
	}
}

void GameScrew_Draw(void)
{
	for (int i = 0; i < g_ScrewSpawnDataCount; i++) {
		switch (g_Screw[i].state)
		{
		case STATE_NONE:
			break;

		case STATE_BAD:
			Sprite_Draw(g_Screw[i].texture_id, g_Screw[i].position.x, g_Screw[i].position.y, SCREW_SIZE, SCREW_SIZE,
				0, 0, 886, 886,	SCREW_SIZE / 2, SCREW_SIZE / 2, g_Screw[i].tightness);

			Sprite_Draw(g_TextureId_Gauge1, g_Screw[i].position.x + GAUGE_OUTSIDE_POS_X, g_Screw[i].position.y + GAUGE_OUTSIDE_POS_Y,
				GAUGE_OUTSIDE_WIDTH, GAUGE_OUTSIDE_HEIGHT, 0, 0, 591, 118, D3DCOLOR_RGBA(255, 255, 255, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT, g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT + g_Screw[i].gauge_left_width + GAUGE_POINT_RANGE,
				g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				GAUGE_INSUDE_TOTAL - g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));

			Sprite_Draw(g_TextureId_Gauge3, g_Screw[i].position.x + GAUGE_POINT_POS_X, g_Screw[i].position.y + GAUGE_POINT_POS_Y,
				GAUGE_POINT_WIDTH, GAUGE_POINT_HEIGHT, 0, 0, 30, 146);
			break;
			
		case STATE_GOOD:
			Sprite_Draw(g_Screw[i].texture_id, g_Screw[i].position.x, g_Screw[i].position.y, SCREW_SIZE, SCREW_SIZE,
				0, 0, 886, 886, SCREW_SIZE / 2, SCREW_SIZE / 2, g_Screw[i].tightness);

			Sprite_Draw(g_TextureId_Gauge1, g_Screw[i].position.x + GAUGE_OUTSIDE_POS_X, g_Screw[i].position.y + GAUGE_OUTSIDE_POS_Y,
				GAUGE_OUTSIDE_WIDTH, GAUGE_OUTSIDE_HEIGHT, 0, 0, 591, 118, D3DCOLOR_RGBA(255, 255, 0, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT, g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT + g_Screw[i].gauge_left_width + GAUGE_POINT_RANGE,
				g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				GAUGE_INSUDE_TOTAL - g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));

			Sprite_Draw(g_TextureId_Gauge3, g_Screw[i].position.x + GAUGE_POINT_POS_X, g_Screw[i].position.y + GAUGE_POINT_POS_Y,
				GAUGE_POINT_WIDTH, GAUGE_POINT_HEIGHT, 0, 0, 30, 146);
			break;

		case STATE_SPRING_ON:
			Sprite_Draw(g_Screw[i].texture_id, g_Screw[i].position.x, g_Screw[i].position.y, SCREW_SIZE, SCREW_SIZE,
				0, 0, 886, 886, SCREW_SIZE / 2, SCREW_SIZE / 2, g_Screw[i].tightness);

			Sprite_Draw(g_TextureId_Gauge1, g_Screw[i].position.x + GAUGE_OUTSIDE_POS_X, g_Screw[i].position.y + GAUGE_OUTSIDE_POS_Y,
				GAUGE_OUTSIDE_WIDTH, GAUGE_OUTSIDE_HEIGHT, 0, 0, 591, 118, D3DCOLOR_RGBA(255, 120, 95, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT, g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT + g_Screw[i].gauge_left_width + GAUGE_POINT_RANGE,
				g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				GAUGE_INSUDE_TOTAL - g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));
			break;

		case STATE_SPRING_WORK:
			Sprite_Draw(g_Screw[i].texture_id, g_Screw[i].position.x, g_Screw[i].position.y, SCREW_SIZE, SCREW_SIZE,
				0, 0, 886, 886, SCREW_SIZE / 2, SCREW_SIZE / 2, g_Screw[i].tightness);

			Sprite_Draw(g_TextureId_Gauge1, g_Screw[i].position.x + GAUGE_OUTSIDE_POS_X, g_Screw[i].position.y + GAUGE_OUTSIDE_POS_Y,
				GAUGE_OUTSIDE_WIDTH, GAUGE_OUTSIDE_HEIGHT, 0, 0, 591, 118, D3DCOLOR_RGBA(255, 120, 95, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT, g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT + g_Screw[i].gauge_left_width + GAUGE_POINT_RANGE,
				g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				GAUGE_INSUDE_TOTAL - g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));
			break;

		case STATE_SPRING_OFF:
			Sprite_Draw(g_Screw[i].texture_id, g_Screw[i].position.x, g_Screw[i].position.y, SCREW_SIZE, SCREW_SIZE,
				0, 0, 886, 886, SCREW_SIZE / 2, SCREW_SIZE / 2, g_Screw[i].tightness);

			Sprite_Draw(g_TextureId_Gauge1, g_Screw[i].position.x + GAUGE_OUTSIDE_POS_X, g_Screw[i].position.y + GAUGE_OUTSIDE_POS_Y,
				GAUGE_OUTSIDE_WIDTH, GAUGE_OUTSIDE_HEIGHT, 0, 0, 591, 118, D3DCOLOR_RGBA(255, 255, 255, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT, g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));

			Sprite_Draw(g_TextureId_Gauge2, g_Screw[i].position.x + GAUGE_INSUDE_POS_X_LEFT + g_Screw[i].gauge_left_width + GAUGE_POINT_RANGE,
				g_Screw[i].position.y + GAUGE_INSUDE_POS_Y,
				GAUGE_INSUDE_TOTAL - g_Screw[i].gauge_left_width, GAUGE_INSIDE_HEIGHT, 0, 0, 578, 106, D3DCOLOR_RGBA(128, 128, 128, 255));
			break;

		case STATE_SWITCH_ON:
			Sprite_Draw(g_Screw[i].texture_id, g_Screw[i].position.x, g_Screw[i].position.y, SCREW_SIZE, SCREW_SIZE,
				0, 0, 886, 886, SCREW_SIZE / 2, SCREW_SIZE / 2, g_Screw[i].tightness);
			Sprite_Draw(g_TextureId_Lamp_On, g_Screw[i].position.x + LAMP_POS_X, g_Screw[i].position.y - LAMP_POS_Y,
				LAMP_SIZE, LAMP_SIZE, 0, 0, 349, 350);
			break;

		case STATE_SWITCH_OFF:
			Sprite_Draw(g_Screw[i].texture_id, g_Screw[i].position.x, g_Screw[i].position.y, SCREW_SIZE, SCREW_SIZE,
				0, 0, 886, 886, SCREW_SIZE / 2, SCREW_SIZE / 2, g_Screw[i].tightness);
			Sprite_Draw(g_TextureId_Lamp_Off, g_Screw[i].position.x + LAMP_POS_X, g_Screw[i].position.y - LAMP_POS_Y, 
				LAMP_SIZE, LAMP_SIZE, 0, 0, 350, 349);
			break;
		}

	}
}

CircleCollision GameScrew_GetCollision(int i)
{
	CircleCollision ScrewCollision;
	ScrewCollision.center.x = g_Screw[i].position.x + SCREW_SIZE / 2;
	ScrewCollision.center.y = g_Screw[i].position.y + SCREW_SIZE / 2;
	ScrewCollision.radius = SCREW_SIZE / 2 * 1.5f;	// 少し大きめに当たり判定取る
	return ScrewCollision;
}

bool GameScrew_isEnable(int i)
{
	return g_Screw[i].state != STATE_NONE;
}

Screw* GameScrew_GetScrew(int i)
{
	return &g_Screw[i];
}

int GameScrew_GetMax(void)
{
	return SCREW_MAX;
}

size_t GameScrew_GetCount(void)
{
	return g_ScrewSpawnDataCount;
}

int GameScrew_GetRemain(void)
{
	return g_ScrewClearCount - g_GoodCount;
}

float GameScrew_GetSize(void)
{
	return SCREW_SIZE;
}

bool GameScrew_Turned(int index, float angle)
{
	g_Screw[index].tightness += angle;

	// ただのスイッチネジ、ジャンプ台ネジはいくらでも回せるので常にreturn true
	if (g_Screw[index].state == STATE_SWITCH_ON || g_Screw[index].state == STATE_SWITCH_OFF) {
		return true;
	}

	if (g_Screw[index].state == STATE_SPRING_ON || g_Screw[index].state == STATE_SPRING_OFF 
		|| g_Screw[index].state == STATE_SPRING_WORK) {
		// 赤or青のブロックを動かす
		GameScaffold_Move(g_Screw[index].gimmick_color, g_Screw[index].tightness);
	}

	if (g_Screw[index].tightness < -TIGHTNESS_MAX) {
		g_Screw[index].tightness = -TIGHTNESS_MAX;
	}
	else if (g_Screw[index].tightness > TIGHTNESS_MAX) {
		g_Screw[index].tightness = TIGHTNESS_MAX;
	}

	if (fabsf(g_Screw[index].tightness) >= TIGHTNESS_MAX) {
		return false;
	}
	else {
		return true;
	}
}

void GameScrew_Translation(void)
{
	for (int i = 0; i < g_ScrewSpawnDataCount; i++) {
		g_Screw[i].position.x += -GamePlayer_GetPlayer()->velocity.x;
	}
}

void GameScrew_AutoTurned(int index, float angle)
{
	switch (g_Screw[index].gimmick_color)
	{
	case GIMMICK_NONE:
		break;
	case GIMMICK_RED:
		GameScrew_AutoTurnedR(index, angle);
		break;
	case GIMMICK_GREEN:
		GameScrew_AutoTurnedG(index, angle);
		break;
	case GIMMICK_BLUE:
		GameScrew_AutoTurnedB(index, angle);
		break;
	case GIMMICK_YELLOW:
		GameScrew_AutoTurnedY(index, angle);
		break;
	}
}

void GameScrew_AutoTurnedR(int index, float angle)
{
	if (g_Screw[index].tightness > 0) {
		g_Screw[index].tightness -= angle;
	}
	else {
		g_Screw[index].tightness += angle;
	}

	if (fabsf(g_Screw[index].tightness) < angle) {
		g_Screw[index].tightness = 0.0f;
		GameScrew_ToSpringOff(index);
	}

	if (g_Screw[index].state == STATE_SPRING_ON || g_Screw[index].state == STATE_SPRING_OFF
		|| g_Screw[index].state == STATE_SPRING_WORK) {
		// 赤or青のブロックを動かす
		GameScaffold_Move(g_Screw[index].gimmick_color, g_Screw[index].tightness);
	}
}

void GameScrew_AutoTurnedG(int index, float angle)
{
	g_Screw[index].tightness += angle;

	if (g_Screw[index].tightness > TIGHTNESS_MAX) {
		g_Screw[index].tightness = TIGHTNESS_MAX;
		GameScrew_ToSpringOff(index);
		GameScaffold_Switch(g_Screw[index].gimmick_color);
	}
}

void GameScrew_AutoTurnedB(int index, float angle)
{
	if (g_Screw[index].tightness > 0) {
		g_Screw[index].tightness -= angle;
	}
	else {
		g_Screw[index].tightness += angle;
	}

	if (fabsf(g_Screw[index].tightness) < angle) {
		g_Screw[index].tightness = 0.0f;
		GameScrew_ToSpringOff(index);
	}

	if (g_Screw[index].state == STATE_SPRING_ON || g_Screw[index].state == STATE_SPRING_OFF
		|| g_Screw[index].state == STATE_SPRING_WORK) {
		// 赤or青のブロックを動かす
		GameScaffold_Move(g_Screw[index].gimmick_color, g_Screw[index].tightness);
	}
}

void GameScrew_AutoTurnedY(int index, float angle)
{
	g_Screw[index].tightness -= angle;

	if (g_Screw[index].tightness < -TIGHTNESS_MAX) {
		g_Screw[index].tightness = -TIGHTNESS_MAX;
		GameScrew_ToSpringOff(index);
		GameScaffold_Switch(g_Screw[index].gimmick_color);
	}
}

// 同じギミックカラーのぜんまいねじが一つでも作動していたらスイッチねじはオン、そうでなければオフ
void GameScrew_Switch(int index)
{
	switch (g_Screw[index].gimmick_color)
	{
		case GIMMICK_RED:
			g_Screw[index].state = g_RedOnNumber >= 1 ? STATE_SWITCH_ON : STATE_SWITCH_OFF;
			break;
		case GIMMICK_GREEN:
			g_Screw[index].state = g_GreenOnNumber >= 1 ? STATE_SWITCH_ON : STATE_SWITCH_OFF;
			break;
		case GIMMICK_BLUE:
			g_Screw[index].state = g_BlueOnNumber >= 1 ? STATE_SWITCH_ON : STATE_SWITCH_OFF;
			break;
		case GIMMICK_YELLOW:
			g_Screw[index].state = g_YellowOnNumber >= 1 ? STATE_SWITCH_ON : STATE_SWITCH_OFF;
			break;
	}
}

void GameScrew_ToSpringOn(int index)
{
	g_Screw[index].state = STATE_SPRING_ON;
}

void GameScrew_ToSpringWorkFromOn(int index)
{
	g_Screw[index].state = STATE_SPRING_WORK;
}

void GameScrew_ToSpringWorkFromOff(int index)
{
	g_Screw[index].state = STATE_SPRING_WORK;

	switch (g_Screw[index].gimmick_color)
	{
	case GIMMICK_RED:
		g_RedOnNumber++;
		break;
	case GIMMICK_GREEN:
		g_GreenOnNumber++;
		break;
	case GIMMICK_BLUE:
		g_BlueOnNumber++;
		break;
	case GIMMICK_YELLOW:
		g_YellowOnNumber++;
		break;
	}
}

void GameScrew_ToSpringOff(int index)
{
	g_Screw[index].state = STATE_SPRING_OFF;

	switch (g_Screw[index].gimmick_color)
	{
	case GIMMICK_RED:
		g_RedOnNumber--;
		break;
	case GIMMICK_GREEN:
		g_GreenOnNumber--;
		break;
	case GIMMICK_BLUE:
		g_BlueOnNumber--;
		break;
	case GIMMICK_YELLOW:
		g_YellowOnNumber--;
		break;
	}
}

void GameScrew_ToAllSpringOff(int index)
{
	for (int i = 0; i < g_ScrewSpawnDataCount; i++) {
		// 同じギミックカラーのネジを探す
		if (g_Screw[i].state == STATE_SPRING_ON && g_Screw[i].gimmick_color == g_Screw[index].gimmick_color) {
			g_Screw[i].state = STATE_SPRING_OFF;
		}
	}
	switch (g_Screw[index].gimmick_color)
	{
	case GIMMICK_RED:
		g_RedOnNumber = 0;
		break;
	case GIMMICK_GREEN:
		g_GreenOnNumber = 0;
		break;
	case GIMMICK_BLUE:
		g_BlueOnNumber = 0;
		break;
	case GIMMICK_YELLOW:
		g_YellowOnNumber = 0;
		break;
	}
}

//void GameScrew_SpringSwitch(int index)
//{
//
//	for (int i = 0; i < g_ScrewSpawnDataCount; i++) {
//		// 同じギミックカラーのネジを探す
//		if (g_Screw[i].gimmick_color != g_Screw[index].gimmick_color) {
//			continue;
//		}
//		switch (g_Screw[i].state)
//		{
//		case STATE_SPRING_ON:
//			g_Screw[i].state = STATE_SPRING_OFF;
//			break;
//		case STATE_SPRING_OFF:
//			g_Screw[i].state = STATE_SPRING_ON;
//			break;
//		case STATE_SWITCH_ON:
//			g_Screw[i].state = STATE_SWITCH_OFF;
//			break;
//		case STATE_SWITCH_OFF:
//			g_Screw[i].state = STATE_SWITCH_ON;
//			break;
//		default:
//			break;
//		}
//	}
//}

void GameScrew_Reset(void)
{
	// 全てを未使用、ギミック無し状態に初期化しておく
	for (int i = 0; i < SCREW_MAX; i++) {
		g_Screw[i].state = STATE_NONE;
	}

	// ステージ番号に合わせてScrewSpawnDataをセット
	g_pScrewSpawnData = g_pScrewSpawn[g_NowStage];
	g_ScrewSpawnDataCount = g_numScrew[g_NowStage];

	// クリア条件に関わるネジの個数を初期化、後で数える
	g_ScrewClearCount = 0;

	// g_Screwをg_ScrewSpawnDataで設定した個数分有効にし、それぞれ指定した値を入れていく
	for (int i = 0; i < g_ScrewSpawnDataCount; i++) {
		g_Screw[i].position = D3DXVECTOR2(START_X + SCREW_SIZE * g_pScrewSpawnData[i].column, SCREW_SIZE * g_pScrewSpawnData[i].row);
		g_Screw[i].state = g_pScrewSpawnData[i].state;
		g_Screw[i].gimmick_color = g_pScrewSpawnData[i].gimmick_color;
		// ギミックの種類に応じて決められた値を格納
		// クリア条件に関わるネジ(GIMMICK_NONE)を数える
		switch (g_Screw[i].gimmick_color)
		{
		case GIMMICK_NONE:
			g_Screw[i].texture_id = g_TextureId_Screw;
			g_Screw[i].tightness = g_pScrewSpawnData[i].tightness;
			g_ScrewClearCount++;
			break;
		case GIMMICK_RED:
			g_Screw[i].texture_id = g_TextureId_Screw_Red;
			g_Screw[i].tightness = TIGHTNESS_MAX * 0.0f;
			break;
		case GIMMICK_GREEN:
			g_Screw[i].texture_id = g_TextureId_Screw_Green;
			g_Screw[i].tightness = TIGHTNESS_MAX * 1.0f;
			break;
		case GIMMICK_BLUE:
			g_Screw[i].texture_id = g_TextureId_Screw_Blue;
			g_Screw[i].tightness = TIGHTNESS_MAX * 0.0f;
			break;
		case GIMMICK_YELLOW:
			g_Screw[i].texture_id = g_TextureId_Screw_Yellow;
			g_Screw[i].tightness = TIGHTNESS_MAX * -1.0f;
			break;
		}
		g_Screw[i].gauge_left_width =
			GAUGE_INSUDE_TOTAL * (TIGHTNESS_MAX - g_Screw[i].tightness) / (TIGHTNESS_MAX * 2);
	}

	g_GoodCount = 0;

	g_RedOnNumber = 0;
	g_GreenOnNumber = 0;
	g_BlueOnNumber = 0;
	g_YellowOnNumber = 0;
}