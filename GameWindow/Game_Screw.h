/*==============================================================================

	�Q�[���̃l�W�֌W�̏����@[Game_Screw.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 21
==============================================================================*/
#ifndef GAME_SCREW_H_
#define GAME_SCREW_H_

#include "Config.h"
#include "Collision.h"

#define SCREW_SIZE				(30.0f)
#define START_X					(-20.0f)

#define TIGHTNESS_MAX			(D3DXToRadian(360.0f * 10))
#define TIGHTNESS_DECREASE		(D3DXToRadian(10.0f))	// ����܂��l�W�������ŉ�鎞�̑��x

#define SCREW_MAX_X				(SCREEN_WIDTH / (int)SCREW_SIZE)
#define SCREW_MAX_Y				(SCREEN_HEIGHT / (int)SCREW_SIZE)
#define SCREW_MAX				(SCREW_MAX_X * SCREW_MAX_Y)

#define TIGHTNESS_GOOD_RANGE	(D3DXToRadian(360.0f * 3))

// �Q�[�W�̊O��
#define	GAUGE_OUTSIDE_WIDTH		(SCREW_SIZE * 2.0f)
#define	GAUGE_OUTSIDE_HEIGHT	(GAUGE_OUTSIDE_WIDTH / 4.0f)
// �eScrew�̍��W����Ƃ����ꍇ�̃Q�[�W�̊O���̑��Έʒu
#define	GAUGE_OUTSIDE_POS_X		(SCREW_SIZE / 2 - GAUGE_OUTSIDE_WIDTH / 2)
#define	GAUGE_OUTSIDE_POS_Y		(-GAUGE_OUTSIDE_HEIGHT * 1.5f)

// �Q�[�W�̓���
#define GAUGE_INSIDE_WIDTH_DEFAULT	(GAUGE_OUTSIDE_WIDTH / 40 * 39)
#define GAUGE_INSIDE_HEIGHT			(GAUGE_OUTSIDE_HEIGHT * 9 / 10)
// �eScrew�̍��W����Ƃ����ꍇ�̃Q�[�W�̓����̑��Έʒu
#define	GAUGE_INSUDE_POS_X_LEFT		(GAUGE_OUTSIDE_POS_X + (GAUGE_OUTSIDE_WIDTH - GAUGE_INSIDE_WIDTH_DEFAULT) / 2)
#define	GAUGE_INSUDE_POS_X_RIGHT	(GAUGE_OUTSIDE_POS_X)
#define	GAUGE_INSUDE_POS_Y			(GAUGE_OUTSIDE_POS_Y + (GAUGE_OUTSIDE_HEIGHT / 10) / 2)

// �Q�[�W�̃C�C�����|�C���g
#define GAUGE_POINT_WIDTH			(GAUGE_OUTSIDE_WIDTH / 10)
#define GAUGE_POINT_HEIGHT			(GAUGE_OUTSIDE_HEIGHT * 1.5f)
// �eScrew�̍��W����Ƃ����ꍇ�̃Q�[�W�̃C�C�����|�C���g�̑��Έʒu
#define GAUGE_POINT_POS_X			(SCREW_SIZE / 2 - GAUGE_POINT_WIDTH / 2)
#define GAUGE_POINT_POS_Y			(GAUGE_OUTSIDE_POS_Y - GAUGE_POINT_HEIGHT / 3)

#define GAUGE_POINT_RANGE			(SCREW_SIZE / 8 * 3)
#define	GAUGE_INSUDE_TOTAL			(GAUGE_INSIDE_WIDTH_DEFAULT - GAUGE_POINT_RANGE)

// �X�C�b�`�l�W�̃����v
#define LAMP_SIZE					(SCREW_SIZE / 2)
#define LAMP_POS_X					(SCREW_SIZE / 4)
#define LAMP_POS_Y					(SCREW_SIZE / 4 * 3)

// �l�W�̏��
typedef enum ScrewState_tag {
	STATE_NONE,			// �g���ĂȂ����
	STATE_BAD,			// �C�C�����łȂ����
	STATE_GOOD,			// �C�C�����̏��
	STATE_SPRING_ON,	// ����܂������Ă���
	STATE_SPRING_WORK,	// ����܂��ڍs��ԁi�v���C���[�������Ă��ԁj
	STATE_SPRING_OFF,	// ����܂������ĂȂ����
	STATE_SWITCH_ON,	// �X�C�b�`�I��
	STATE_SWITCH_OFF,	// �X�C�b�`�I�t
}ScrewState;

// �M�~�b�N�̐F�i�����F�̃M�~�b�N���A���j
typedef enum GimmickColor_tag {
	GIMMICK_NONE,	// �M�~�b�N����
	GIMMICK_RED,
	GIMMICK_BLUE,
	GIMMICK_GREEN,
	GIMMICK_YELLOW,
}GimmickColor;

/*------------------------------------------------------------------------------
	�\���̐錾
------------------------------------------------------------------------------*/
// �l�W�\����
typedef struct Screw_tag
{
	D3DXVECTOR2 position;		// ���W
	ScrewState state;			// ���
	float tightness;			// ���܂��i0����ɏ������Ȃ�ƒ��܂�A�傫���Ȃ�Ɗɂށj
	float gauge_left_width;		// �����̃Q�[�W�̉����i�E���̃Q�[�W�̉����͈����Z�ŎZ�o�j
	CircleCollision collision;	// �R���W����
	GimmickColor gimmick_color;	// �M�~�b�N�J���[
	int texture_id;				// �e�N�X�`��ID�i�M�~�b�N�J���[�ɂ���ăe�N�X�`�����قȂ邽�߁j
}Screw;

void GameScrew_Initialize(void);
void GameScrew_Finalize(void);
void GameScrew_Update(void);
void GameScrew_Draw(void);

CircleCollision GameScrew_GetCollision(int i);

bool GameScrew_isEnable(int i);

Screw* GameScrew_GetScrew(int i);

int GameScrew_GetMax(void);

size_t GameScrew_GetCount(void);

int GameScrew_GetRemain(void);

float GameScrew_GetSize(void);

bool GameScrew_Turned(int index, float angle);

void GameScrew_Translation(void);

void GameScrew_AutoTurned(int index, float angle);
void GameScrew_AutoTurnedR(int index, float angle);
void GameScrew_AutoTurnedG(int index, float angle);
void GameScrew_AutoTurnedB(int index, float angle);
void GameScrew_AutoTurnedY(int index, float angle);

// �����M�~�b�N�J���[�̂���܂��˂�����ł��쓮���Ă�����X�C�b�`�˂��̓I���A�����łȂ���΃I�t
void GameScrew_Switch(int index);

// ����܂��˂��̏�ԑJ�ڊ֐�
void GameScrew_ToSpringOn(int index);
void GameScrew_ToSpringWorkFromOn(int index);
void GameScrew_ToSpringWorkFromOff(int index);
void GameScrew_ToSpringOff(int index);
void GameScrew_ToAllSpringOff(int index);

//void GameScrew_SpringSwitch(int index);

void GameScrew_Reset(void);

#endif // GAME_SCREW_H_