/*==============================================================================

	�Q�[���̊e�픻�菈���@[Game_Judgment.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 12 / 28
									
==============================================================================*/
#include "Game_Judgment.h"
#include "Game_Player.h"

void GameJudgment_Initialize(void)
{
	
}

void GameJudgment_Update(void)
{
	GameJudgment_ScrewToDriver();
}

// �l�W�ƃh���C�o�[�Ƃ̓����蔻��
int GameJudgment_ScrewToDriver(void) 
{
	// �l�W�ƃh���C�o�[�Ƃ̓����蔻�肪�K�v�Ȃ̂̓v���C���[���W�����v��Ԃ̎��݂̂ł��邽�߃W�����v��ԈȊO�Ȃ�return
	//if (GamePlayer_GetPlayer()->state != STATE_JUMP) {
	//	// �������ĂȂ��Ƃ݂Ȃ���-1��Ԃ�
	//	return -1;
	//}

	// ���s��Ԃ̎����l�W�ɂ������悤�ɂ����p�^�[��
	// ���̏ꍇ�l�W�ƃh���C�o�[�Ƃ̓����蔻�肪�K�v�Ȃ��̂̓v���C���[����]��Ԃ̎��݂̂ł��邽�߉�]��ԂȂ�return
	if (GamePlayer_GetPlayer()->state == STATE_TURN) {
		// �������ĂȂ��Ƃ݂Ȃ���-1��Ԃ�
		return -1;
	}

	for (int i = 0; i < GameScrew_GetCount(); i++) {
		// ���݂����L�������ׂ�(���̂Ƃ���S���L�������ǂ�����ꎞ�I�ɖ����ɂȂ������ԕς�����肷�邩������񂩂炱��͎c��)
		if (!GameScrew_isEnable(i)) {
			continue;
		}

		// ���݂����L���������瓖���蔻��
		if (!CollisionCircleAndCircleHit(&GamePlayer_GetDriverCollision(), &GameScrew_GetCollision(i))) {
			continue;
		}

		// �������Ă���l�W�̃C���f�b�N�X��Ԃ�
		return i;
	}

	// �������ĂȂ�������-1��Ԃ�
	return -1;
}