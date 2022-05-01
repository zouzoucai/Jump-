/*==============================================================================

	direct3D�f�o�C�X�̐ݒ菈���@[Mydirect3d.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Config.h"

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static LPDIRECT3D9			g_pD3D = NULL;		// Direct3D�C���^�[�t�F�[�X
static LPDIRECT3DDEVICE9	g_pDevice = NULL;	// Direct3D�f�o�C�X�C���^�[�t�F�[�X

/*------------------------------------------------------------------------------
	�֐���`
------------------------------------------------------------------------------*/

// direct3d9�̏�����
// ���s������false��Ԃ�
bool MyDirect3D_Initialize(HWND hWnd)
{
	// Direct3D�C���^�[�t�F�[�X�̎擾
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL) {
		MessageBox(NULL, "Direct3D�C���^�[�t�F�[�X�̎擾�Ɏ��s���܂����B", "�G���[", MB_OK);
		return false;
	}

	/* Direct3D�f�o�C�X�̎擾 */

	// �f�o�C�X�̎擾�ɕK�v�ȏ��\���̂̍쐬
	D3DPRESENT_PARAMETERS d3dpp = {};			// 0�ŏ�����
	d3dpp.BackBufferWidth = SCREEN_WIDTH;		// �o�b�N�o�b�t�@�̉���
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;		// �o�b�N�o�b�t�@�̏c��
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	// �o�b�N�o�b�t�@�̃t�H�[�}�b�g�i�f�X�N�g�b�v�ɂ��܂����j
	d3dpp.BackBufferCount = 1;					// �o�b�N�o�b�t�@�̐�
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// �X���b�v���@�̐ݒ�
	d3dpp.Windowed = TRUE;						// �E�C���h�E or �t���X�N���[���i�t���Ȃ�|�b�v�A�b�v�E�C���h�E�������߁j
	d3dpp.EnableAutoDepthStencil = TRUE;		// �[�x�o�b�t�@�E�X�e���V���o�b�t�@�̎g�p
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	// �[�x�o�b�t�@�E�X�e���V���o�b�t�@�̃t�H�[�}�b�g
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; // �t���X�N���[�����̃��t���b�V�����[�g�̎w��
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// ���t���b�V�����[�g��present�����̊֌W
//	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// �����Ȃ�i�댯�j�ACPU�Ɉˑ�


	// Direct3D�f�o�C�X�̎擾
	HRESULT hr;
	hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pDevice);

	if (FAILED(hr)) {
		MessageBox(NULL, "Direct3D�f�o�C�X�̎擾�Ɏ��s���܂����B", "�G���[", MB_OK);
		return false;
	}

	// UV�A�h���b�V���O���[�h�̐ݒ�
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// ���u�����h�̐ݒ�
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ���_�J���[�ƃe�N�X�`���̃u�����h�ݒ�
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	return true;	// ����������

}


// direct3dp�̏I������
void MyDirect3D_Finalize(void)
{
	if (g_pDevice) {
		g_pDevice->Release();
		g_pDevice = NULL;
	}

	if (g_pD3D) {
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

LPDIRECT3DDEVICE9 MyDirect3D_GetDevice(void)
{
	return g_pDevice;
}