/*==============================================================================

	�Q�[���̃S�[�������@[Game_Goal.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 25
==============================================================================*/
#include <d3dx9.h>
#include <stdio.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Game_Player.h"
#include "StageSelect.h"
#include "my_font.h"

#define GOAL_WIDTH	(SCREW_SIZE)
#define GOAL_HEIGHT	(SCREW_SIZE * 2.0f)

#define NUM_SIZE	(SCREW_SIZE)	// �c��l�W���̕\���T�C�Y

/*------------------------------------------------------------------------------
	�\���̐錾
------------------------------------------------------------------------------*/
// �S�[���ʒu�Ǘ��\����
typedef struct GoalSpawn_tag {
	int row;						// �s
	int column;						// ��
}GoalSpawn;

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static int g_TextureIdGoalBad = TEXTURE_INVALID_ID;			// �S�[��(�N���A�s�\)
static int g_TextureIdGoalGood = TEXTURE_INVALID_ID;		// �S�[��(�N���A�\)
static int g_TextureIdGoalOpen = TEXTURE_INVALID_ID;		// �S�[��(�󂢂Ă���)

static bool g_isOpen = false;	// �S�[�����󂢂Ă��邩�ǂ���

static bool g_isClear = false;	// �N���A�������ǂ���

static D3DXVECTOR2 g_GoalPosition(0.0f, 0.0f);

static int g_NowStage = 0;	// ���݂̃X�e�[�W

// �e�X�e�[�W���̃S�[���̈ʒu�i�s�Ɨ�Ŏw��j
static GoalSpawn g_Position[] = {
	{22, 29},	// �`���[�g���A���X�e�[�W
	{11, 35},	// �X�e�[�W1
	{9, 2},	// �X�e�[�W2
	{8,  39},	// �X�e�[�W3
	{2, 36},	// �X�e�[�W4
	{7,  5},	// �X�e�[�W5
	{22, 38},	// �X�e�[�W6
	{12, 41},	// �X�e�[�W7
	{ 2,  4},	// �X�e�[�W8
    {22, 21},	// �X�e�[�W9
	{3,  4},	// �X�e�[�W10
	{22, 36},	// �X�e�[�W11
	{3,  22},	// �X�e�[�W12
	{12, 37},	// �X�e�[�W13
	{ 3, 33},	// �X�e�[�W14
	{18, 40},	// �X�e�[�W15
	{22, 40},	// �X�e�[�W16
	{16, 23},	// �X�e�[�W17
	{22, 22},	// �X�e�[�W18
	{22, 21},	// �X�e�[�W19
	{22, 39},	// �X�e�[�W20
};

void GameGoal_Initialize(void)
{
	// �e�N�X�`���̓ǂݍ��ݗ\��
	g_TextureIdGoalBad = Texture_SetTextureLoadFile("asset/texture/goal_bad.png");
	g_TextureIdGoalGood = Texture_SetTextureLoadFile("asset/texture/goal_good.png");
	g_TextureIdGoalOpen = Texture_SetTextureLoadFile("asset/texture/goal_open.png");

	// �e�N�X�`���̓ǂݍ���
	if (Texture_Load() > 0) {
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B", "�G���[", MB_OK);
		return;
	}

	// �X�e�[�W�ԍ����Z�b�g
	g_NowStage = StageSelect_GetStageNumber();

	g_GoalPosition = D3DXVECTOR2(START_X + g_Position[g_NowStage].column * SCREW_SIZE, g_Position[g_NowStage].row * SCREW_SIZE);

	g_isOpen = false;

	g_isClear = false;
}

void GameGoal_Finalize(void)
{
	Texture_Release(&g_TextureIdGoalBad, 1);
	Texture_Release(&g_TextureIdGoalGood, 1);
	Texture_Release(&g_TextureIdGoalOpen, 1);
}

bool GameGoal_Update(void)
{
	if (!g_isOpen) {
		return false;
	}

	Player* pPlayer = GamePlayer_GetPlayer();

	// �v���C���[�����s��ԂłȂ����return false
	if (pPlayer->state != STATE_WALK) {
		return false;
	}

	// �S�[���ɓ����Ă��Ȃ����return false
	if (pPlayer->position.y < g_GoalPosition.y
		|| pPlayer->position.y > g_GoalPosition.y
		|| pPlayer->position.x + PLAYER_WIDTH < g_GoalPosition.x + GOAL_WIDTH / 1.5f
		|| pPlayer->position.x > g_GoalPosition.x + GOAL_WIDTH / 3) {
		return false;
	}
	else {
		g_isClear = true;
		return true;
	}

}

void GameGoal_Draw(void)
{
	D3DCOLOR num_color = D3DCOLOR_RGBA(255, 255, 255, 255);
	int remain = GameScrew_GetRemain();

	char buf[4];
	sprintf(buf, "%d", remain);
	if (remain >= 10) {
		MyFont_Draw(g_GoalPosition.x - NUM_SIZE / 2, g_GoalPosition.y - NUM_SIZE, NUM_SIZE, NUM_SIZE, buf, num_color);
	}
	else {
		MyFont_Draw(g_GoalPosition.x, g_GoalPosition.y - NUM_SIZE, NUM_SIZE, NUM_SIZE, buf, num_color);
	}

	if (!g_isOpen) {
		Sprite_Draw(g_TextureIdGoalBad, g_GoalPosition.x, g_GoalPosition.y, GOAL_WIDTH, GOAL_HEIGHT, 0, 0, 180, 360);
	}
	else if(g_isOpen && !g_isClear){
		Sprite_Draw(g_TextureIdGoalGood, g_GoalPosition.x, g_GoalPosition.y, GOAL_WIDTH, GOAL_HEIGHT, 0, 0, 180, 360);
	}
	else {
		Sprite_Draw(g_TextureIdGoalOpen, g_GoalPosition.x, g_GoalPosition.y, GOAL_WIDTH, GOAL_HEIGHT, 0, 0, 180, 360);
	}
}

void GameGoal_ToEnable(void)
{
	g_isOpen = true;
}

void GameGoal_ToDisenable(void)
{
	g_isOpen = false;
}
int GameGoal_GetNowStage(void)
{
	return g_NowStage;
}