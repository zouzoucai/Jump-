/*==============================================================================

	�e�N�X�`�������@[Texture.h]

													Author : Sai Hakusen
													Date   : 2020 / 12 / 15
==============================================================================*/
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <d3d9.h>

#define TEXTURE_FILENAME_MAX (64)	// �e�N�X�`���t�@�C�����̍ő啶����
#define TEXTURE_INVALID_ID (-1)		// �����ȃe�N�X�`���Ǘ��ԍ�

// �������̈����ɓ��I�������m�ۂŊǗ��ő吔�Ƃ������Ɨǂ�
void Texture_Initialize(void);
// ��{�g��Ȃ����Ǔ��I�������m�ۂ��Ȃ炱���Ō�Еt��
void Texture_Finalize(void);

// �e�N�X�`���t�@�C���̓ǂݍ��ݗ\��
// ���� pFileName...�t�@�C����
// �߂�l �e�N�X�`���Ǘ��ԍ�...�ő�Ǘ����𒴂��Ă����ꍇ��INVALID_TEXTURE_ID
int Texture_SetTextureLoadFile(const char* pFileName);

// �e�N�X�`���̓ǂݍ���
// �\�񂳂�Ă���t�@�C����ǂݍ��݂܂�
// �߂�l �ǂݍ��߂Ȃ������t�@�C�����܂��͑��̕s������������ꍇ-1
// 
int Texture_Load(void);

// int textureIds[] => int* pTextureIds
// �e�N�X�`���̕����J��(�����������܂�H��Ȃ��悤�ɂ���)
// ���� textureIds[]...�J��������textureId��������int�^�z��̐擪�A�h���X
//      count...�J������e�N�X�`���̐�
void Texture_Release(int textureIds[], int count);

// �e�N�X�`���̑S�J��
void Texture_AllRelease(void);

unsigned long Texture_GetTextureWidth(int textureId);
unsigned long Texture_GetTextureHeight(int textureId);

// �e�N�X�`���C���^�t�F�[�X�|�C���^�̎擾
// �ǂݍ��߂Ă��Ȃ��܂��͕s���ȊǗ��ԍ��������ꍇ��NULL
LPDIRECT3DTEXTURE9 Texture_GetTexture(int textureId);



#endif // TEXTURE_H_