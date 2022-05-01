/*==============================================================================

	�Q�[���̑��ꏈ���@[Game_Scaffold.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 21
==============================================================================*/
#ifndef GAME_SCAFFOLD_H_
#define GAME_SCAFFOLD_H_

#include "Game_Screw.h"

typedef enum ScaffoldType_tag {
	TYPE_NONE,				// ���g�p
	TYPE_BLOCK_ON,			// �u���b�N�iON�j
	TYPE_BLOCK_OFF,			// �u���b�N�iOFF�j
	TYPE_FLOOR,				// ��
}ScaffoldType;

#define SCAFFOLD_MAX	(SCREW_MAX)
#define SCAFFOLD_SIZE	(SCREW_SIZE)
#define FLOOR_HEIGHT	(SCAFFOLD_SIZE / 5)

/*------------------------------------------------------------------------------
	�\���̐錾
------------------------------------------------------------------------------*/
// ����\����
typedef struct Scaffold_tag
{
	D3DXVECTOR2 position;			// ���W
	D3DXVECTOR2 initial_position;	// �������W�i�������Ƃ��̊�j
	float width;					// ��
	float height;					// ����
	int numHorizontal;				// ���̐�
	int numVertical;				// �c�̐�(���͂ǂ̐�������Ă�1�ŌŒ�)
	ScaffoldType type;				// �^�C�v
	GimmickColor gimmick_color;		// �M�~�b�N�J���[
	int texture_id;					// �e�N�X�`��ID�i�M�~�b�N�J���[�A�I�����I�t���ɂ���ăe�N�X�`�����قȂ邽�߁j
	int texture_id_off;				// �e�N�X�`��ID�i�M�~�b�N�J���[�A�I�����I�t���ɂ���ăe�N�X�`�����قȂ邽�߁j
}Scaffold;

void GameScaffold_Initialize(void);
void GameScaffold_Finalize(void);
void GameScaffold_Update(void);
void GameScaffold_Draw(void);

Scaffold* GameScaffold_GetScaffold(int i);

size_t GameScaffold_GetCount(void);

// gimmickColor�ɉ���������̃I���I�t��؂�ւ���
void GameScaffold_Switch(GimmickColor gimmickColor);

// gimmickColor�ɉ���������𓮂���
void GameScaffold_Move(GimmickColor gimmickColor, float tightness);

void GameScaffold_Reset(void);

#endif // GAME_SCAFFOLD_H_