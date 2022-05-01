/*==============================================================================

	�f�o�b�O�p�}�`�\�������@[DebugPrimitive.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#if defined(_DEBUG) || defined(DEBUG)	// �f�o�b�O�̎��������s

#include <d3dx9.h>
#include <math.h>
#include "Mydirect3d.h"

#define CIRCLE_VERTEX_COUNT	(64)	// 64�p�`
#define CIRCLE_DRAW_MAX		(2048)	// 2048��64�p�`

typedef struct DebugVertex_tag
{
	D3DXVECTOR4 position;
	D3DCOLOR color;
}DebugVertex;
#define FVF_DEBUG_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

static LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
static LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;

// ����3�͖��t���[�������������
static int g_CircleCount = 0;
static DebugVertex* g_pDebugVertex = NULL;
static WORD* g_pDebugVertexIndex = NULL;

#endif // _DEBUG || DEBUG

void DebugPrimitive_Initialize(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	// �e�O���[�o���ϐ��̏�����
	g_CircleCount = 0;

	// ���_�o�b�t�@��C���f�b�N�X�o�b�t�@�̊m��
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();

	pDevice->CreateVertexBuffer(
		sizeof(DebugVertex) * CIRCLE_VERTEX_COUNT * CIRCLE_DRAW_MAX,
		D3DUSAGE_WRITEONLY,		// �g����(����͏��������œǂ܂Ȃ����珈������)
		FVF_DEBUG_VERTEX,		// FVF
		D3DPOOL_MANAGED,		// �������̊Ǘ����@(DEFAULT���Ǝ��������ŕ��������Ȃ�����G���o�Ȃ��̂Œ���)
		&g_pVertexBuffer,		// �擾�����C���^�[�t�F�[�X�̃A�h���X���L�^���邽�߂̃|�C���^�ւ̃A�h���X
		NULL
	);

	pDevice->CreateIndexBuffer(
		sizeof(WORD) * CIRCLE_VERTEX_COUNT * 2 * CIRCLE_DRAW_MAX,
		D3DUSAGE_WRITEONLY,		// �g����(����͏��������œǂ܂Ȃ����珈������)
		D3DFMT_INDEX16,			// FVF
		D3DPOOL_MANAGED,		// �������̊Ǘ����@(DEFAULT���Ǝ��������ŕ��������Ȃ�����G���o�Ȃ��̂Œ���)
		&g_pIndexBuffer,
		NULL
	);

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_Finalize(void)
{
#if defined(_DEBUG) || defined(DEBUG)

	if (g_pIndexBuffer) {
		g_pIndexBuffer->Release();
		g_pIndexBuffer = NULL;
	}

	if (g_pVertexBuffer) {
		g_pVertexBuffer->Release();
		g_pVertexBuffer = NULL;
	}

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_BatchBegin(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	// ���_�o�b�t�@�ƒ��_�C���f�b�N�X�o�b�t�@�����b�N����
	g_pVertexBuffer->Lock(0, 0, (void**)&g_pDebugVertex, 0);
	g_pIndexBuffer->Lock(0, 0, (void**)&g_pDebugVertexIndex, 0);

	// �ۂ̕`�施�ߐ���������
	g_CircleCount = 0;

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_BatchDrawCircle(float x, float y, float radius)
{
#if defined(_DEBUG) || defined(DEBUG)
	// �w����W���~�̒��S�Ƃ����ۂ̒��_���W���v�Z���āA
	// ���_�o�b�t�@�ƒ��_�C���f�b�N�X�o�b�t�@�ɏ��������

	// �z��̂ǂ����珑������邩
	WORD n = (WORD)(g_CircleCount * CIRCLE_VERTEX_COUNT);

	// �O�p�`����́i���W�A���p�j
	float s = D3DX_PI * 2.0f / CIRCLE_VERTEX_COUNT;

	// ���_�o�b�t�@�ƒ��_�C���f�b�N�X�o�b�t�@�֏�����������
	for (WORD i = 0; i < CIRCLE_VERTEX_COUNT; i++) {
		g_pDebugVertex[n + i].position.y = sinf(s * i) * radius + y;
		g_pDebugVertex[n + i].position.x = cosf(s * i) * radius + x;
		g_pDebugVertex[n + i].position.z = 1.0f;
		g_pDebugVertex[n + i].position.w = 1.0f;
		g_pDebugVertex[n + i].color = D3DCOLOR_RGBA(0, 0, 0, 255); //�F�̎w������Ō��߂���悤�ɂ���̂��A��

		g_pDebugVertexIndex[n * 2 + i * 2] = n + i;
		g_pDebugVertexIndex[n * 2 + i * 2 + 1] = n + (i + 1) % CIRCLE_VERTEX_COUNT;
	}

	// �ۂ̕`�施�ߐ��𑝂₷
	g_CircleCount++;

#endif // _DEBUG || DEBUG
}

void DebugPrimitive_BatchRun(void)
{
#if defined(_DEBUG) || defined(DEBUG)
	// ���_�o�b�t�@�ƒ��_�C���f�b�N�X�o�b�t�@���A�����b�N����
	g_pVertexBuffer->Unlock();
	g_pIndexBuffer->Unlock();

	// �ۂ̕`�施�ߐ����̊ۂ�`��
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	
	int vn = g_CircleCount * CIRCLE_VERTEX_COUNT;

	pDevice->SetFVF(FVF_DEBUG_VERTEX);
	pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(DebugVertex));
	pDevice->SetIndices(g_pIndexBuffer);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, vn, 0, vn);
#endif // _DEBUG || DEBUG
}