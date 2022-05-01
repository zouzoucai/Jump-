/*==============================================================================

	�Q�[���̃v���C���[�֌W�̏����@[Game_Player.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 27
==============================================================================*/
#include <d3dx9.h>
#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Keylogger.h"
#include "Scene.h"
#include "Fade.h"
#include "Game_Player.h"
#include "Game_Judgment.h"
#include "Game_Scaffold.h"
#include "Controller.h"
#include "StageSelect.h"
#include "sound.h"

#define DIRECTION_WIDTH		(40.0f)
#define DIRECTION_HEIGHT	(20.0f)

#define GRAVITY_WEAK_FRAME	(30)	// ��]�W�����v����̐������グ�邽�ߏd�͂���߂�t���[����

/*------------------------------------------------------------------------------
	�\���̐錾
------------------------------------------------------------------------------*/
// �v���C���[�����ʒu�Ǘ��\����
typedef struct PlayerSpawn_tag {
	int row;						// �s
	int column;						// ��
	// ��ԓ���Ă�����������
}PlayerSpawn;

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static int g_TextureIdPlayer = TEXTURE_INVALID_ID;		// ���s��
static int g_TextureIdPlayer2 = TEXTURE_INVALID_ID;		// �W�����v��
static int g_TextureIdPlayer3 = TEXTURE_INVALID_ID;		// ��]��
static int g_TextureIdDriver = TEXTURE_INVALID_ID;
static int g_TextureIdDirection = TEXTURE_INVALID_ID;

static Player g_Player;

static int g_NowStage = 0;	// ���݂̃X�e�[�W

// �e�X�e�[�W���̃v���C���[�̏����ʒu�i�s�Ɨ�Ŏw��j
static PlayerSpawn g_InitialPosition[] = {
	{22, 2},	// �`���[�g���A���X�e�[�W
	{12, 5},	// �X�e�[�W1
	{7, 39},	// �X�e�[�W2
	{20, 1},	// �X�e�[�W3
	{22, 3},	// �X�e�[�W4
	{22, 7},	// �X�e�[�W5
	{22, 3},	// �X�e�[�W6
	{6, 5},	// �X�e�[�W7
	{18,31},	// �X�e�[�W8
	{1, 16},	// �X�e�[�W9
	{14,34},	// �X�e�[�W10
	{18,36},	// �X�e�[�W11
	{19,22},	// �X�e�[�W12
	{17, 8},	// �X�e�[�W13
	{ 9, 3},	// �X�e�[�W14
	{ 6, 6},	// �X�e�[�W15
	{22, 3},	// �X�e�[�W16
	{16, 23},	// �X�e�[�W17
	{3, 3},	// �X�e�[�W18
	{22, 21},	// �X�e�[�W19
	{10, 12},	// �X�e�[�W20
};

static D3DXVECTOR2 g_DriverPosition = D3DXVECTOR2(0.0f, 0.0f);
static CircleCollision g_DriverCollision;

static D3DXVECTOR2 g_DirectionPosition = D3DXVECTOR2(0.0f, 0.0f);
static float g_DirectionAngle = 0.0f;

static float g_gravity = 0.4f;

static int g_ScrewIndex = -1;	// �������Ă���l�W�̃C���f�b�N�X�A�l�W�ɂ������Ă��Ȃ��ꍇ��-1

static float g_WalkFrame = 0;	// ���s���̃A�j���[�V�����p�ϐ�

static float g_rad = 0.0f;		// �W�����v���̉�]���[�V�����p�ϐ�

static int g_FrameCounter = 0;

void GamePlayer_Initialize(void)
{
	// �e�N�X�`���̓ǂݍ��ݗ\��
	g_TextureIdPlayer = Texture_SetTextureLoadFile("asset/texture/�L�����A�j���[����3_3(�k����01).png");
	g_TextureIdPlayer2 = Texture_SetTextureLoadFile("asset/texture/�L�������01.png");
	g_TextureIdPlayer3 = Texture_SetTextureLoadFile("asset/texture/�L�����n���O��1�i�h���C�o�Ȃ��j.png");
	g_TextureIdDriver = Texture_SetTextureLoadFile("asset/texture/cude.png");
	g_TextureIdDirection = Texture_SetTextureLoadFile("asset/texture/player_direction.png");

	// �e�N�X�`���̓ǂݍ���
	if (Texture_Load() > 0) {
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B", "�G���[", MB_OK);
		return;
	}

	// �X�e�[�W�ԍ����Z�b�g
	g_NowStage = StageSelect_GetStageNumber();

	GamePlayer_Reset();
}

void GamePlayer_Finalize(void)
{
	Texture_Release(&g_TextureIdPlayer, 1);
	Texture_Release(&g_TextureIdPlayer2, 1);
	Texture_Release(&g_TextureIdPlayer3, 1);
	Texture_Release(&g_TextureIdDriver, 1);
	Texture_Release(&g_TextureIdDirection, 1);
}

void GamePlayer_Update(void)
{
	g_Player.position_old = g_Player.position;

	switch (g_Player.state)
	{
	case STATE_WALK:
		// �W�����v
		if (Controller_Decision() && g_Player.velocity.y == 0.0f) {
			g_Player.velocity.y = PLAYER_JUMP_SPEED;
			g_Player.state = STATE_JUMP;
			PlaySound(SOUND_LABEL_SE_JUMP);
			break;
		}

		// �n��ł̈ړ��i���s�j
		GamePlayer_Walk();

		// �h���C�o�[�˂��h��
		if (GameJudgment_ScrewToDriver() >= 0) {
			PlaySound(SOUND_LABEL_SE_CATCH);
			GamePlayer_CatchScrew();
			break;
		}

		// �A�j���[�V�����p�R�}�X�V����
		if (fabsf(g_Player.velocity.x) > 0.1f) {
			g_WalkFrame += 1.0f / 2.0f * fabsf(g_Player.velocity.x) / PLAYER_WALK_SPEED_MAX;
		}

		break;
	case STATE_JUMP:
		// �h���C�o�[�˂��h��
		if (GameJudgment_ScrewToDriver() >= 0 && (g_FrameCounter - g_Player.turn_jump_frame > 15)) {
			PlaySound(SOUND_LABEL_SE_CATCH);
			GamePlayer_CatchScrew();
			break;
		}

		g_rad += 0.3f;

		// �󒆂ł̈ړ�
		GamePlayer_Move();

		break;
	case STATE_TURN:
		// �R���g���[���[�œ��͂����������x�N�g���Ŏ擾
		D3DXVECTOR2 jump_direction =
		{ (Controller_GetLeftStickX() / 65535.0f) - 0.5f, (Controller_GetLeftStickY() / 65535.0f) - 0.5f };

		switch(Controller_GetDirectionButton())
		{
		case BUTTON_UP:
			jump_direction = { 0.0f, -0.5f };
			break;
		case BUTTON_UPRIGHT:
			jump_direction = { 0.25f, -0.25f };
			break;
		case BUTTON_RIGHT:
			jump_direction = { 0.5f,  0.0f };
			break;
		case BUTTON_DOWNRIGHT:
			jump_direction = { 0.25f, 0.25f };
			break;
		case BUTTON_DOWN:
			jump_direction = { 0.0f,  0.5f };
			break;
		case BUTTON_DOWNLEFT:
			jump_direction = {-0.25f,  0.25f };
			break;
		case BUTTON_LEFT:
			jump_direction = {-0.5f,   0.0f };
			break;
		case BUTTON_UPLEFT:
			jump_direction = {-0.25f, -0.25f };
			break;
		default:
			break;
		}

		// ��]�W�����v�A�l�W���痣���
		if (Controller_Decision()) {
			GamePlayer_ReleaseScrew(jump_direction);
			break;
		}

		// ���������̉�]
		GamePlayer_Turn();

		// ���̕��������߂�
		if ((fabsf(jump_direction.x) >= 0.1f || fabsf(jump_direction.y) >= 0.1f) && Controller_isGamePad()) {
			g_DirectionAngle = atan2f(jump_direction.y, jump_direction.x) - D3DXToRadian(90);
		}
		else {
			g_DirectionAngle = g_Player.angle;
		}

		// �������Ă�Screw�̉��̂ݖ炷
		Screw* pScrew = GameScrew_GetScrew(g_ScrewIndex);
		if (((int)pScrew->tightness) % 3 == 0) {
			PlaySound(SOUND_LABEL_SE_SCREW_TURN_02);
		}

		break;
	}

	// ��ʊO�ɏo�Ȃ��悤�ɂ��鏈��
	if (g_Player.position.x < 0.0f) {
		g_Player.position.x = 0.0f;
	}
	else if (g_Player.position.x > SCREEN_WIDTH - PLAYER_WIDTH) {
		g_Player.position.x = SCREEN_WIDTH - PLAYER_WIDTH;
	}

	g_FrameCounter++;
}

void GamePlayer_Draw(void)
{
	switch (g_Player.state)
	{
	case STATE_WALK:
		if (g_Player.direction == RIGHT) {
			Sprite_Draw(g_TextureIdPlayer, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT,
				145 * ((int)g_WalkFrame % 4), 0, 145, 280);
		}
		else {
			Sprite_Draw(g_TextureIdPlayer, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT,
				(145 * ((int)g_WalkFrame % 4)) + 145, 0, -145, 280);
		}
		break;
	case STATE_JUMP:
		if (g_Player.direction == RIGHT) {
			Sprite_Draw(g_TextureIdPlayer2, g_Player.position.x, g_Player.position.y, PLAYER_SIZE, PLAYER_SIZE, 0, 0, 1600, 1600,
				PLAYER_SIZE / 2, PLAYER_SIZE / 2, g_rad);
		}
		else {
			Sprite_Draw(g_TextureIdPlayer2, g_Player.position.x, g_Player.position.y, PLAYER_SIZE, PLAYER_SIZE, 1600, 0, -1600, 1600,
				PLAYER_SIZE / 2, PLAYER_SIZE / 2, -g_rad);
		}
		break;
	case STATE_TURN:
		if (g_Player.direction == RIGHT) {
			Sprite_Draw(g_TextureIdPlayer3, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT, 0, 0, 1400, 2800,
				PLAYER_WIDTH / 2, -DRIVER_SIZE / 2, g_Player.angle);
		}
		else {
			Sprite_Draw(g_TextureIdPlayer3, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT, 1400, 0, -1400, 2800,
				PLAYER_WIDTH / 2, -DRIVER_SIZE / 2, g_Player.angle);
		}

		Sprite_Draw(g_TextureIdDriver, g_DriverPosition.x, g_DriverPosition.y, DRIVER_SIZE, DRIVER_SIZE, 0, 0, 505, 478,
			DRIVER_SIZE / 2, DRIVER_SIZE / 2, g_Player.angle);
		
		// ���\��
		Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 0, 255));
		Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
			DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT), g_DirectionAngle);

		// �p���x�ŕ\������Direction�̐��A�F��ς���
		if (fabsf(g_Player.ang_velocity) > PLAYER_TURN_SPEED_MAX * 0.95f) {
			Sprite_SetColor(D3DCOLOR_RGBA(255, 110, 0, 255));
			Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y + DIRECTION_HEIGHT, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
				DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT + DIRECTION_HEIGHT), g_DirectionAngle);
			Sprite_SetColor(D3DCOLOR_RGBA(255, 0, 0, 255));
			Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y + DIRECTION_HEIGHT * 2, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
				DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT + DIRECTION_HEIGHT * 2), g_DirectionAngle);
		}
		else if (fabsf(g_Player.ang_velocity) > PLAYER_TURN_SPEED_MAX * 0.75f) {
			Sprite_SetColor(D3DCOLOR_RGBA(255, 110, 0, 255));
			Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y + DIRECTION_HEIGHT, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
				DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT + DIRECTION_HEIGHT), g_DirectionAngle);
		}
		Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
		break;
	}
}

// �n��ł̃v���C���[�̈ړ�
void GamePlayer_Walk(void)
{
	if (Controller_MoveLeft()) {
		//g_Player.velocity.x = -PLAYER_WALK_SPEED;
		// �������ɉ����A�ō����x�ɒB����������͎~�܂�
		g_Player.velocity.x = g_Player.velocity.x > -PLAYER_WALK_SPEED_MAX ?
			g_Player.velocity.x - PLAYER_WALK_ACCELERATION : -PLAYER_WALK_SPEED_MAX;
		g_Player.direction = LEFT;
	}

	if (Controller_MoveRight()) {
		//g_Player.velocity.x = PLAYER_WALK_SPEED;
		// �E�����ɉ����A�ō����x�ɒB����������͎~�܂�
		g_Player.velocity.x = g_Player.velocity.x < PLAYER_WALK_SPEED_MAX ?
			g_Player.velocity.x + PLAYER_WALK_ACCELERATION : PLAYER_WALK_SPEED_MAX;
		g_Player.direction = RIGHT;
	}

	g_Player.velocity.x *= 0.9f;

	// �d�͂ŉ��ɗ������鏈��
	g_Player.velocity.y += g_gravity;

	// ���x�̕������ړ�
	g_Player.position += g_Player.velocity;

	// �h���C�o�[�̍��W�̓v���C���[�ɒǏ]����
	if (g_Player.direction == RIGHT) {
		g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
		g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
	}
	else {
		g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
		g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
	}

	// ����Ƃ̓����蔻��
	for (int i = 0; i < GameScaffold_GetCount(); i++) {
		Scaffold* pScaffold = GameScaffold_GetScaffold(i);
		switch (pScaffold->type)
		{
		case TYPE_NONE:
			break;
		case TYPE_BLOCK_ON:
			// �Փ˂��Ă��Ȃ����break
			if (g_Player.position.y + PLAYER_HEIGHT <= pScaffold->position.y
				|| g_Player.position.y >= pScaffold->position.y + pScaffold->height
				|| g_Player.position.x + PLAYER_WIDTH <= pScaffold->position.x
				|| g_Player.position.x >= pScaffold->position.x + pScaffold->width) {
				break;
			}

			// �㍶�E�ǂ̕�������̏Փ˂Ȃ̂�����
			// �e���莮�̍Ō�̐�����position_old���u���b�N�ɂ߂荞�񂾎��ł��������悤�ɂ��邽�߂̂���
			// �ォ��̏ꍇ
			if (g_Player.position_old.y + PLAYER_HEIGHT <= pScaffold->position.y + 5.0f) {
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
				g_Player.velocity = D3DXVECTOR2(g_Player.velocity.x, 0.0f);
			}
			// ������̏ꍇ
			else if (g_Player.position_old.x + PLAYER_WIDTH <= pScaffold->position.x + 15.0f) {
				g_Player.position.x = pScaffold->position.x - PLAYER_WIDTH;
			}
			// �E����̏ꍇ
			else if (g_Player.position_old.x >= pScaffold->position.x + pScaffold->width - 15.0f) {
				g_Player.position.x = pScaffold->position.x + pScaffold->width;
			}

			break;
		case TYPE_FLOOR:
			// �Փ˂��Ă��Ȃ����break
			if (g_Player.position.y + PLAYER_HEIGHT < pScaffold->position.y
				|| g_Player.position.y > pScaffold->position.y + pScaffold->height
				|| g_Player.position.x + PLAYER_WIDTH < pScaffold->position.x
				|| g_Player.position.x > pScaffold->position.x + pScaffold->width) {
				break;
			}

			// �ォ��̏ꍇ(���͏ォ��̔���̂�)
			if (g_Player.position_old.y + PLAYER_HEIGHT <= pScaffold->position.y) {
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
				g_Player.velocity = D3DXVECTOR2(g_Player.velocity.x, 0.0f);
			}
			break;
		}
	}

	if (g_Player.position.y > SCREEN_HEIGHT - PLAYER_HEIGHT) {
		g_Player.position.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
		g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
		g_Player.velocity.y = 0.0f;
	}
}

// �󒆂ł̃v���C���[�̈ړ�
void GamePlayer_Move(void)
{
	if (Controller_MoveLeft() && (g_FrameCounter - g_Player.turn_jump_frame > 10)) {
		// �������ɉ����A�ō����x�ɒB����������͎~�܂�
		g_Player.velocity.x = g_Player.velocity.x > -PLAYER_MOVE_SPEED_MAX ?
			g_Player.velocity.x - PLAYER_MOVE_ACCELERATION : -PLAYER_MOVE_SPEED_MAX;
	}

	if (Controller_MoveRight() && (g_FrameCounter - g_Player.turn_jump_frame > 10)) {
		// �E�����ɉ����A�ō����x�ɒB����������͎~�܂�
		g_Player.velocity.x = g_Player.velocity.x < PLAYER_MOVE_SPEED_MAX ?
			g_Player.velocity.x + PLAYER_MOVE_ACCELERATION : PLAYER_MOVE_SPEED_MAX;
	}

	g_Player.velocity.x *= 0.98f;

	// �d�͂ŉ��ɗ������鏈��
	if (g_FrameCounter - g_Player.turn_jump_frame > GRAVITY_WEAK_FRAME) {
		g_Player.velocity.y += g_gravity;
	}
	else if (g_FrameCounter - g_Player.turn_jump_frame > GRAVITY_WEAK_FRAME / 3 * 2) {
		g_Player.velocity.y += g_gravity * 0.75f;
	}
	else if (g_FrameCounter - g_Player.turn_jump_frame > GRAVITY_WEAK_FRAME / 3) {
		g_Player.velocity.y += g_gravity * 0.5f;
	}

	// ���x�̕������ړ�
	g_Player.position += g_Player.velocity;

	// �h���C�o�[�̍��W�̓v���C���[�ɒǏ]����
	g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 3.5f;
	g_DriverPosition.y = g_Player.position.y + 5.0f;

	// ����Ƃ̓����蔻��
	for (int i = 0; i < GameScaffold_GetCount(); i++) {
		Scaffold* pScaffold = GameScaffold_GetScaffold(i);
		switch (pScaffold->type)
		{
		case TYPE_NONE:
			break;
		case TYPE_BLOCK_ON:
			// �Փ˂��Ă��Ȃ����break
			if (g_Player.position.y + PLAYER_SIZE <= pScaffold->position.y
				|| g_Player.position.y >= pScaffold->position.y + pScaffold->height
				|| g_Player.position.x + PLAYER_SIZE <= pScaffold->position.x
				|| g_Player.position.x >= pScaffold->position.x + pScaffold->width) {
				break;
			}

			// �㉺���E�ǂ̕�������̏Փ˂Ȃ̂�����
			// ���O�̍��W���ǂ����������ŏՓ˂̈ʒu�𔻒�
			// �ォ��̏ꍇ
			if (g_Player.position_old.y + PLAYER_SIZE <= pScaffold->position.y + 5.0f && g_Player.velocity.y > 0) {
				// ���ɂ͂ݏo�Ȃ��悤�ɂ��鏈��
				if (g_Player.position.x + PLAYER_WIDTH / 2 < pScaffold->position.x) {
					g_Player.position.x = pScaffold->position.x - PLAYER_WIDTH / 2;
				}
				// �E�ɂ͂ݏo�Ȃ��悤�ɂ��鏈��
				else if (g_Player.position.x > pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2) {
					g_Player.position.x = pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2;
				}
				g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				GamePlayer_ToWalk();
			}
			// ������̏ꍇ
			else if (g_Player.position_old.y >= pScaffold->position.y + pScaffold->height - 5.0f) {
				g_Player.position.y = pScaffold->position.y + pScaffold->height;
			}
			// ������̏ꍇ
			else if (g_Player.position_old.x + PLAYER_SIZE <= pScaffold->position.x + 5.0f) {
				g_Player.position.x = pScaffold->position.x - PLAYER_SIZE;
			}
			// �E����̏ꍇ
			else if (g_Player.position_old.x >= pScaffold->position.x + pScaffold->width - 5.0f) {
				g_Player.position.x = pScaffold->position.x + pScaffold->width;
			}
			
			break;
		case TYPE_FLOOR:
			// �Փ˂��Ă��Ȃ����break
			if (g_Player.position.y + PLAYER_SIZE < pScaffold->position.y
				|| g_Player.position.y > pScaffold->position.y + FLOOR_HEIGHT
				|| g_Player.position.x + PLAYER_SIZE < pScaffold->position.x
				|| g_Player.position.x > pScaffold->position.x + pScaffold->width) {
				break;
			}

			// �ォ��̏ꍇ(���͏ォ��̔���̂�)
			if (g_Player.position_old.y + PLAYER_SIZE <= pScaffold->position.y && g_Player.velocity.y > 0) {
				// ���ɂ͂ݏo�Ȃ��悤�ɂ��鏈��
				if (g_Player.position.x + PLAYER_WIDTH / 2 < pScaffold->position.x) {
					g_Player.position.x = pScaffold->position.x - PLAYER_WIDTH / 2;
				}
				// �E�ɂ͂ݏo�Ȃ��悤�ɂ��鏈��
				else if (g_Player.position.x > pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2) {
					g_Player.position.x = pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2;
				}
				g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				GamePlayer_ToWalk();
			}

			break;
		}
	}

	// ����艺�܂ŗ������璅�n
	if (g_Player.position.y < 0.0f) {
		g_Player.position.y = 0.0f;
	}
	else if (g_Player.position.y > SCREEN_HEIGHT - PLAYER_SIZE) {
		g_Player.position.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
		GamePlayer_ToWalk();
	}
}

// �h���C�o�[���l�W�Ɏh�������̃v���C���[�̉�]
void GamePlayer_Turn(void)
{
	// ��]���x�A�O���͈�C�ɏオ��A�㔼�͂������オ��
	if (Controller_TurnLeft()) {
		// ���v�������ɉ����A�ō����x�ɒB����������͎~�܂�
		if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX * 0.0f) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.1f;
		}
		else if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX * 0.25f) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.05f;
		}
		else if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.01f;
		}
		else if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.003f;
		}
		else {
			g_Player.ang_velocity = PLAYER_TURN_SPEED_MAX;
		}
	}

	if (Controller_TurnRight()) {
		// �����v�������ɉ����A�ō����x�ɒB����������͎~�܂�
		if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX * 0.0f) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.1f;
		}
		else if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX * 0.25f) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.05f;
		}
		else if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.01f;
		}
		else if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.003f;
		}
		else {
			g_Player.ang_velocity = -PLAYER_TURN_SPEED_MAX;
		}
	}

	if (!Controller_TurnLeft() && !Controller_TurnRight()) {
		if (sinf(g_Player.angle) > 0) {
			g_Player.ang_velocity -= D3DXToRadian(0.2f);
		}
		else if (sinf(g_Player.angle) < 0) {
			g_Player.ang_velocity += D3DXToRadian(0.2f);
		}
		g_Player.ang_velocity *= 0.995f;
	}

	// GameScrew_Turned��Screw���܂��񂹂�ꍇ��true�A�񂹂Ȃ��ꍇ��false��Ԃ��֐��ł���A
	// if���̏������ɓ���邱�Ƃ�Screw��tightness��ω������Ȃ���񂹂邩�ǂ����̔�������Ă���
	if (GameScrew_Turned(g_ScrewIndex, g_Player.ang_velocity)) {
		g_Player.angle += g_Player.ang_velocity;
	}
	else {
		g_Player.ang_velocity = 0.0f;
	}

	// g_Player.angle���n�ʂƐ����Ȋp�x���班�������ς��Ȃ��ꍇ�ɁA�p�x��0�ɂ���
	// ���ꂪ�Ȃ��Ƃ��΂炭�̊ԃv���C���[�������݂ɐk���邽��
	g_Player.angle = cosf(g_Player.angle) < 0.9999f ? g_Player.angle : 0.0f;
}

Player* GamePlayer_GetPlayer(void)
{
	return &g_Player;
}

CircleCollision GamePlayer_GetPlayerCollision(void)
{
	return g_Player.collision;
}

CircleCollision GamePlayer_GetDriverCollision(void)
{
	CircleCollision DriverCollision;

	DriverCollision.center.x = g_DriverPosition.x + DRIVER_SIZE / 2;
	DriverCollision.center.y = g_DriverPosition.y + DRIVER_SIZE / 2;
	DriverCollision.radius = DRIVER_SIZE / 2 * 1.5f;	// �����傫�߂ɓ����蔻����

	return DriverCollision;
}

void GamePlayer_CatchScrew(void)
{
	// �������l�W�̃C���f�b�N�X���擾
	g_ScrewIndex = GameJudgment_ScrewToDriver();
	g_Player.state = STATE_TURN;
	g_DriverPosition = GameScrew_GetScrew(g_ScrewIndex)->position +
		D3DXVECTOR2((SCREW_SIZE - DRIVER_SIZE) / 2, (SCREW_SIZE - DRIVER_SIZE) / 2);
	if (g_Player.direction == RIGHT) {
		g_Player.position = g_DriverPosition + D3DXVECTOR2(DRIVER_SIZE / 2 - PLAYER_WIDTH / 2, DRIVER_SIZE);
	}
	else {
		g_Player.position = g_DriverPosition + D3DXVECTOR2(DRIVER_SIZE / 2 - PLAYER_WIDTH / 2, DRIVER_SIZE);
	}
	g_DirectionPosition = g_DriverPosition + D3DXVECTOR2(DRIVER_SIZE / 2 - DIRECTION_WIDTH / 2, DRIVER_SIZE + PLAYER_HEIGHT);
	// �˂��h�����u�Ԃ̊p���x�̓v���C���[��x�����̑��x��-10�{�ɔ��
	g_Player.ang_velocity = D3DXToRadian(g_Player.velocity.x / PLAYER_MOVE_SPEED_MAX * -10.0f);

	// �l�W�̏�Ԃɍ��킹�ăl�W��M�~�b�N�ւ̏�����ς���
	Screw* pScrew = GameScrew_GetScrew(g_ScrewIndex);
	switch (pScrew->state)
	{
	case STATE_SPRING_OFF:
		GameScrew_ToSpringWorkFromOff(g_ScrewIndex);
		GameScaffold_Switch(pScrew->gimmick_color);
		break;
	case STATE_SPRING_ON:
		GameScrew_ToSpringWorkFromOn(g_ScrewIndex);
		break;
	case STATE_SWITCH_ON:
		GameScrew_ToAllSpringOff(g_ScrewIndex);
		break;
	}
}

void GamePlayer_ReleaseScrew(D3DXVECTOR2 jump_direction)
{
	g_Player.state = STATE_JUMP;
	// �X�e�B�b�N��x�����܂���y�����̂����ꂩ���������l�ȏ�ɓ����Ă�����X�e�B�b�N��|���Ă�������ɔ��
	if ((fabsf(jump_direction.x) >= 0.1f || fabsf(jump_direction.y) >= 0.1f) && Controller_isGamePad()) {
		D3DXVec2Normalize(&jump_direction, &jump_direction);
		if (fabsf(g_Player.ang_velocity) < PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.velocity = D3DXVECTOR2(
				jump_direction.x * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_X,
				jump_direction.y * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_Y);
		}
		else {
			g_Player.velocity = D3DXVECTOR2(
				jump_direction.x * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_X,
				jump_direction.y * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_Y);
		}
	}
	else {
		if (fabsf(g_Player.ang_velocity) < PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.velocity = D3DXVECTOR2(
				cosf(D3DXToRadian(90.0f) + g_Player.angle) * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_X,
				sinf(D3DXToRadian(90.0f) + g_Player.angle) * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_Y);
		}
		else {
			g_Player.velocity = D3DXVECTOR2(
				cosf(D3DXToRadian(90.0f) + g_Player.angle) * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_X,
				sinf(D3DXToRadian(90.0f) + g_Player.angle) * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_Y);
		}
	}

	g_Player.position = g_DriverPosition +
		D3DXVECTOR2(DRIVER_SIZE / 2 + cosf(D3DXToRadian(90.0f) + g_Player.angle) * DRIVER_SIZE / 2 - PLAYER_SIZE / 2,
			DRIVER_SIZE / 2 + sinf(D3DXToRadian(90.0f) + g_Player.angle) * DRIVER_SIZE / 2 - PLAYER_SIZE / 2);

	g_Player.angle = 0.0f;
	g_Player.ang_velocity = 0.0f;
	g_Player.turn_jump_frame = g_FrameCounter;
	g_DirectionAngle = 0.0f;

	// �l�W�̏�Ԃɍ��킹�ăl�W��M�~�b�N�ւ̏�����ς���
	// ���͂���܂��l�W���쓮������̂�
	Screw* pScrew = GameScrew_GetScrew(g_ScrewIndex);
	if (pScrew->state == STATE_SPRING_WORK) {
		GameScrew_ToSpringOn(g_ScrewIndex);
	}
	g_ScrewIndex = -1;
}

void GamePlayer_ToWalk(void)
{
	if (g_Player.velocity.x > 0.0f) {
		g_Player.direction = RIGHT;
	}
	else {
		g_Player.direction = LEFT;
	}

	g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
	g_Player.velocity = D3DXVECTOR2(g_Player.velocity.x / 2, 0.0f);
	g_Player.state = STATE_WALK;
	g_WalkFrame = 0.0f;
}

void GamePlayer_Reset(void)
{
	g_Player.position = D3DXVECTOR2(START_X + g_InitialPosition[g_NowStage].column * SCREW_SIZE,
		g_InitialPosition[g_NowStage].row * SCREW_SIZE);
	g_Player.position_old = g_Player.position;
	g_Player.velocity = D3DXVECTOR2(0.0f, 0.0f);

	g_Player.state = STATE_WALK;
	g_Player.direction = RIGHT;

	g_Player.turn_jump_frame = -GRAVITY_WEAK_FRAME;	// �J�n����GRAVITY_WEAK_FRAME�̊ԂɃW�����v����ƍ�����щ߂���̂�h������

	g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
	g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;

	g_DirectionPosition = D3DXVECTOR2(0.0f, 0.0f);
	g_DirectionAngle = 0.0f;

	g_gravity = 0.4f;

	g_ScrewIndex = -1;

	g_WalkFrame = 0.0f;

	g_FrameCounter = 0;
}