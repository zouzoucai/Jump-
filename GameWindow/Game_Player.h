/*==============================================================================

	�Q�[���̃v���C���[�֌W�̏����@[Game_Player.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 21
==============================================================================*/
#ifndef GAME_PLAYER_H_
#define GAME_PLAYER_H_

#include "Collision.h"
#include "Game_Screw.h"

#define PLAYER_WIDTH				(SCREW_SIZE)			// �n��������͉�]���̃v���C���[�̉���
#define PLAYER_HEIGHT				(PLAYER_WIDTH * 2)		// �n��������͉�]���̃v���C���[�̍���
#define PLAYER_SIZE					(PLAYER_WIDTH)			// �W�����v���̃v���C���[�̃T�C�Y
#define DRIVER_SIZE					(SCREW_SIZE / 4 * 3)	// �h���C�o�[�̃T�C�Y

#define PLAYER_WALK_SPEED_MAX		(5.0f)					// �n��ł̍ō����x
#define PLAYER_WALK_ACCELERATION	(0.5f)					// �n��ł̉����x
#define PLAYER_MOVE_ACCELERATION	(0.2f)					// �󒆂ł̉����x
#define PLAYER_MOVE_SPEED_MAX		(5.0f)					// �󒆂ł̍ō����x
#define PLAYER_JUMP_SPEED			(-10.0f)				// �W�����v�̏���
#define PLAYER_TURN_SPEED_MAX		(D3DXToRadian(12.0f))	// ��]�̍ō����x(���b12��)
#define PLAYER_TURN_JUMP_SPEED_X	(40.0f)					// ��]�W�����v�̏�����x�����̌W��
#define PLAYER_TURN_JUMP_SPEED_Y	(30.0f)					// ��]�W�����v�̏�����y�����̌W��
#define CAMERA_WIDTH				(1000)
#define CAMERA_HEIGHT				(500)

typedef enum PlayerState_tag {
	STATE_WALK,
	STATE_JUMP,
	STATE_TURN,
}PlayerState;

typedef enum PlayerDirection_tag {
	RIGHT,
	LEFT,
}PlayerDirection;

/*------------------------------------------------------------------------------
	�\���̐錾
------------------------------------------------------------------------------*/
// �v���C���[�\����
typedef struct Player_tag
{
	D3DXVECTOR2 position;		// ���W
	D3DXVECTOR2 position_old;	// ���O�̍��W
	D3DXVECTOR2 velocity;		// ���x
	float angle;				// �p�x
	float ang_velocity;			// �p���x
	int turn_jump_frame;		// ��]�W�����v�����u�Ԃ̃t���[��
	PlayerState state;			// ���
	PlayerDirection direction;	// �����i���E�j
	CircleCollision collision;	// �R���W����
}Player;

void GamePlayer_Initialize(void);
void GamePlayer_Finalize(void);
void GamePlayer_Update(void);
void GamePlayer_Draw(void);

void GamePlayer_Walk(void);
void GamePlayer_Move(void);
void GamePlayer_Turn(void);

Player* GamePlayer_GetPlayer(void);

CircleCollision GamePlayer_GetPlayerCollision(void);
CircleCollision GamePlayer_GetDriverCollision(void);

void GamePlayer_CatchScrew(void);
void GamePlayer_ReleaseScrew(D3DXVECTOR2 jump_direction);

void GamePlayer_ToWalk(void);

void GamePlayer_Reset(void);

#endif // GAME_PLAYER_H_