/*==============================================================================

	�X�v���C�g�����@[Sprite.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 06
==============================================================================*/
#include <d3dx9.h>
#include "Mydirect3d.h"
#include "Texture.h"
#include "Sprite.h"

/*------------------------------------------------------------------------------
	�\���̐錾
------------------------------------------------------------------------------*/
// ���_�\����
typedef struct VERTEX2D_tag
{
	D3DXVECTOR4 Position;
	D3DCOLOR	Color;
	D3DXVECTOR2 TexCoord;
}Vertex2D;
#define FVF_VERTEX2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;	// ���_�o�b�t�@�̃C���^�[�t�F�[�X
static LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;	// �C���f�b�N�X�o�b�t�@�̃C���^�[�t�F�[�X

static D3DCOLOR g_color = 0xffffffff;					// �z���C�g

void Sprite_Initialize(void)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		MessageBox(NULL, "�f�o�C�X�̎擾�Ɏ��s���܂����B", "�G���[", MB_OK);
		return;
	}

	pDevice->CreateVertexBuffer(
		sizeof(Vertex2D) * 4,	// ���_�o�b�t�@�̗�(�o�C�g)
		D3DUSAGE_WRITEONLY,		// �g����(����͏��������œǂ܂Ȃ����珈������)
		FVF_VERTEX2D,			// FVF
		D3DPOOL_MANAGED,		// �������̊Ǘ����@(DEFAULT���Ǝ��������ŕ��������Ȃ�����G���o�Ȃ��̂Œ���)
		&g_pVertexBuffer,		// �擾�����C���^�[�t�F�[�X�̃A�h���X���L�^���邽�߂̃|�C���^�ւ̃A�h���X
		NULL
	);

	pDevice->CreateIndexBuffer(
		sizeof(WORD) * 6,		// �C���f�b�N�X�o�b�t�@�̗�(�o�C�g)
		D3DUSAGE_WRITEONLY,		// �g����(����͏��������œǂ܂Ȃ����珈������)
		D3DFMT_INDEX16,			// �t�H�[�}�b�g
		D3DPOOL_MANAGED,		// �������̊Ǘ����@(DEFAULT���Ǝ��������ŕ��������Ȃ�����G���o�Ȃ��̂Œ���)
		&g_pIndexBuffer,		// �擾�����C���^�[�t�F�[�X�̃A�h���X���L�^���邽�߂̃|�C���^�ւ̃A�h���X
		NULL
	);
}

void Sprite_Finalize(void)
{
	if (g_pIndexBuffer) {
		g_pIndexBuffer->Release();
		g_pIndexBuffer = NULL;
	}

	if (g_pVertexBuffer) {
		g_pVertexBuffer->Release();
		g_pVertexBuffer = NULL;
	}
}

// �X�v���C�g�|���S���J���[�ݒ�
// ����:color	�|���S���J���[
void Sprite_SetColor(D3DCOLOR color)
{
	g_color = color;

}

// �X�v���C�g�̕`��
// �e�N�X�`���T�C�Y�ł̕`��
void Sprite_Draw(int textureId, float dx, float dy)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		return;
	}

	// �f�o�C�X��FVF�ݒ�
	pDevice->SetFVF(FVF_VERTEX2D);

	// �f�o�C�X�Ƀe�N�X�`���̐ݒ������
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// �e�N�X�`���̃T�C�Y�擾
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	// ���_�f�[�^�̍쐬
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,     dy - 0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(0.0f,0.0f)},
		{D3DXVECTOR4(dx + w - 0.5f, dy - 0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(1.0f,0.0f)},
		{D3DXVECTOR4(dx - 0.5f,     dy + h - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(0.0f,1.0f)},
		{D3DXVECTOR4(dx + w - 0.5f, dy + h - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(1.0f,1.0f)},
	};

	// ���_�o�b�t�@�̃��b�N�ƃf�[�^�̏�������
	Vertex2D* pV;
	// �������Ƒ�������0�Ȃ�S�����b�N���Ă��Ă����w���ɂȂ�
	g_pVertexBuffer->Lock(0, 0, (void**)&pV, 0);

	memcpy(pV, v, sizeof(v));

	g_pVertexBuffer->Unlock();

	// �C���f�b�N�X�f�[�^
	//unsigned short index[] = {};
	WORD index[] = { 0,1,2,1,3,2 };

	// ���_�o�b�t�@�̃��b�N�ƃf�[�^�̏�������
	WORD* pI;
	// �������Ƒ�������0�Ȃ�S�����b�N���Ă��Ă����w���ɂȂ�
	g_pIndexBuffer->Lock(0, 0, (void**)&pI, 0);
	pI[0] = 0;
	pI[1] = 1;
	pI[2] = 2;
	pI[3] = 1;
	pI[4] = 3;
	pI[5] = 2;

	g_pIndexBuffer->Unlock();

	// ���_�o�b�t�@�̎w��
	pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex2D));

	// �C���f�b�N�X�o�b�t�@�̎w��
	pDevice->SetIndices(g_pIndexBuffer);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}

// �X�v���C�g�̕`��
// �؂���T�C�Y�̃|���S���ŕ`��
// �����@tcx:�e�N�X�`���̐؂���x���W
// �����@tcy:�e�N�X�`���̐؂���y���W
// �����@tcw:�e�N�X�`���̐؂��蕝
// �����@tch:�e�N�X�`���̐؂��荂��
void Sprite_Draw(int textureId, float dx, float dy, int tcx, int tcy, int tcw, int tch)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		return;
	}

	// �f�o�C�X��FVF�ݒ�
	pDevice->SetFVF(FVF_VERTEX2D);

	// �f�o�C�X�Ƀe�N�X�`���̐ݒ������
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// �e�N�X�`���̃T�C�Y�擾
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// ���_�f�[�^�̍쐬
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,       dy +  - 0.5f,    1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dx + tcw - 0.5f, dy +  - 0.5f,    1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(dx - 0.5f,       dy + tch - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dx + tcw - 0.5f, dy + tch - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// ��������؁I�I
	// ���_�o�b�t�@�̃��b�N�ƃf�[�^�̏�������
	Vertex2D* pV;
	// �������Ƒ�������0�Ȃ�S�����b�N���Ă��Ă����w���ɂȂ�
	g_pVertexBuffer->Lock(0, 0, (void**)&pV, 0);

	memcpy(pV, v, sizeof(v));

	g_pVertexBuffer->Unlock();
	
	// ���_�o�b�t�@�̎w��
	pDevice->SetStreamSource(0,g_pVertexBuffer,0,sizeof(Vertex2D));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

// �X�v���C�g�̕`��
// �|���S���̕��E�������w��
// �����@dw;�|���S���T�C�Y��
// �����@dw;�|���S���T�C�Y����
// �����@tcx:�e�N�X�`���̐؂���x���W
// �����@tcy:�e�N�X�`���̐؂���y���W
// �����@tcw:�e�N�X�`���̐؂��蕝
// �����@tch:�e�N�X�`���̐؂��荂��
void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		return;
	}

	// �f�o�C�X��FVF�ݒ�
	pDevice->SetFVF(FVF_VERTEX2D);

	// �f�o�C�X�Ƀe�N�X�`���̐ݒ������
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// �e�N�X�`���̃T�C�Y�擾
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// ���_�f�[�^�̍쐬
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,      dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(dx - 0.5f,      dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// �|���S���̕`��i�ȈՔŁj
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));
}

// �X�v���C�g�̕`��
// �|���S���̕��E�������w��
// �����@dw;�|���S���T�C�Y��
// �����@dw;�|���S���T�C�Y����
// �����@tcx:�e�N�X�`���̐؂���x���W
// �����@tcy:�e�N�X�`���̐؂���y���W
// �����@tcw:�e�N�X�`���̐؂��蕝
// �����@tch:�e�N�X�`���̐؂��荂��
void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch, D3DCOLOR color)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		return;
	}

	// �f�o�C�X��FVF�ݒ�
	pDevice->SetFVF(FVF_VERTEX2D);

	// �f�o�C�X�Ƀe�N�X�`���̐ݒ������
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// �e�N�X�`���̃T�C�Y�擾
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// �����Ŏw�肵���F�ɕς���
	g_color = color;

	// ���_�f�[�^�̍쐬
	Vertex2D v[] = {
		{D3DXVECTOR4(dx - 0.5f,      dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + -0.5f,     1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(dx - 0.5f,      dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dx + dw - 0.5f, dy + dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// �|���S���̕`��i�ȈՔŁj
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));

	// �F�����ɖ߂�
	g_color = 0xffffffff;
}

// �X�v���C�g�̕`��
// �|���S���̕��E�������w��
// �����@dw;�|���S���T�C�Y��
// �����@dw;�|���S���T�C�Y����
// �����@tcx:�e�N�X�`���̐؂���x���W
// �����@tcy:�e�N�X�`���̐؂���y���W
// �����@tcw:�e�N�X�`���̐؂��蕝
// �����@tch:�e�N�X�`���̐؂��荂��
// ����  cx:��]�E�g��̒��S��x���W
// ����  cy:��]�E�g��̒��S��y���W
// ����  angle:�|���S���̉�]�p�x
void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch,
	float cx, float cy, float angle)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		return;
	}

	// �f�o�C�X��FVF�ݒ�
	pDevice->SetFVF(FVF_VERTEX2D);



	// �f�o�C�X�Ƀe�N�X�`���̐ݒ������
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// �e�N�X�`���̃T�C�Y�擾
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// ���_�f�[�^�̍쐬
	Vertex2D v[] = {
		{D3DXVECTOR4(     0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dw - 0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(     0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dw - 0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// ���s�ړ��s��̍쐬
	D3DXMATRIX mtxTranslationC;
	D3DXMatrixTranslation(&mtxTranslationC, -cx, -cy, 0.0f);

	// ���ɖ߂����s�ړ��s��̍쐬
	D3DXMATRIX mtxTranslationI;
	D3DXMatrixTranslation(&mtxTranslationI, cx + dx, cy + dy, 0.0f);

	// ��]�s��̍쐬
	D3DXMATRIX mtxRotation;
	D3DXMatrixRotationZ(&mtxRotation, angle);

	// �g��s��̍쐬
	D3DXMATRIX mtxScale;
	D3DXMatrixScaling(&mtxScale, 1.0f, 1.0f, 1.0f);	// 1�����{�A���̒l�͗��\���t�ɂȂ�

	// �s��̍���
	D3DXMATRIX mtxWorld;
	mtxWorld = mtxTranslationC * mtxScale * mtxRotation * mtxTranslationI;	// *�͍s��̍�����\��(���Ԃɂ͈Ӗ������邽�ߗv���ӁI�I�I�I)

	// ���W�ϊ�
	for (int i = 0; i < 4; i++) {
		D3DXVec4Transform(&v[i].Position, &v[i].Position, &mtxWorld);
	}

	// �|���S���̕`��i�ȈՔŁj
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));
}

void Sprite_Draw(int textureId, float dx, float dy, float dw, float dh, int tcx, int tcy, int tcw, int tch, D3DCOLOR color,
	float cx, float cy, float angle)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		return;
	}

	// �f�o�C�X��FVF�ݒ�
	pDevice->SetFVF(FVF_VERTEX2D);



	// �f�o�C�X�Ƀe�N�X�`���̐ݒ������
	pDevice->SetTexture(0, Texture_GetTexture(textureId));

	// �e�N�X�`���̃T�C�Y�擾
	int w = Texture_GetTextureWidth(textureId);
	int h = Texture_GetTextureHeight(textureId);

	float u0 = (float)tcx / w;
	float v0 = (float)tcy / h;
	float u1 = (float)(tcx + tcw) / w;
	float v1 = (float)(tcy + tch) / h;

	// �����Ŏw�肵���F�ɕς���
	g_color = color;

	// ���_�f�[�^�̍쐬
	Vertex2D v[] = {
		{D3DXVECTOR4(0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v0)},
		{D3DXVECTOR4(dw - 0.5f,     -0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v0)},
		{D3DXVECTOR4(0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u0,v1)},
		{D3DXVECTOR4(dw - 0.5f, dh - 0.5f, 1.0f, 1.0f), g_color,D3DXVECTOR2(u1,v1)},
	};

	// ���s�ړ��s��̍쐬
	D3DXMATRIX mtxTranslationC;
	D3DXMatrixTranslation(&mtxTranslationC, -cx, -cy, 0.0f);

	// ���ɖ߂����s�ړ��s��̍쐬
	D3DXMATRIX mtxTranslationI;
	D3DXMatrixTranslation(&mtxTranslationI, cx + dx, cy + dy, 0.0f);

	// ��]�s��̍쐬
	D3DXMATRIX mtxRotation;
	D3DXMatrixRotationZ(&mtxRotation, angle);

	// �g��s��̍쐬
	D3DXMATRIX mtxScale;
	D3DXMatrixScaling(&mtxScale, 1.0f, 1.0f, 1.0f);	// 1�����{�A���̒l�͗��\���t�ɂȂ�

	// �s��̍���
	D3DXMATRIX mtxWorld;
	mtxWorld = mtxTranslationC * mtxScale * mtxRotation * mtxTranslationI;	// *�͍s��̍�����\��(���Ԃɂ͈Ӗ������邽�ߗv���ӁI�I�I�I)

	// ���W�ϊ�
	for (int i = 0; i < 4; i++) {
		D3DXVec4Transform(&v[i].Position, &v[i].Position, &mtxWorld);
	}

	// �|���S���̕`��i�ȈՔŁj
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vertex2D));

	// �F�����ɖ߂�
	g_color = 0xffffffff;
}