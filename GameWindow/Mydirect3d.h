/*==============================================================================

	direct3D�f�o�C�X�̐ݒ菈���@[Mydirect3d.h]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 15
==============================================================================*/
#ifndef MY_DIRECT3D_H
#define MY_DIRECT3D_H

#include <d3d9.h>

// direct3d9�̏�����
// ���s������false��Ԃ�
bool MyDirect3D_Initialize(HWND hWnd);

// direct3dp�̏I������
void MyDirect3D_Finalize(void);

// Direct3D�f�o�C�X�̎擾
LPDIRECT3DDEVICE9 MyDirect3D_GetDevice(void);

#endif // MY_DIRECT3D_H