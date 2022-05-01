/*==============================================================================

	�f�o�b�O�pprintf�@[debugPrintf.h]
													Author : Ryuichi Tsunoda
													Date   : 2020 / 05 / 18
==============================================================================*/
#ifndef _DEBUG_PRINTF_H_
#define _DEBUG_PRINTF_H_

#include <windows.h>
#include <stdio.h>

// �f�o�b�O�pprintf...VisualStudio�̏o�̓E�B���h�E�ɏo�͂����
inline void DebugPrintf(const char* pFormat, ...)
{
// ����Ȃӂ��ɏ�����Release�r���h���̓r���h����Ȃ��͈͂�ݒ�ł���
#if defined(_DEBUG) || defined(DEBUG)
	va_list argp;
	char buf[256]; // �o�b�t�@�͑傫�߂Ɏ��܂��傤
	va_start(argp, pFormat);
	vsprintf_s(buf, 256, pFormat, argp); //�o�b�t�@�ʂƑ�Q���������킹�邱��
	va_end(argp);
	OutputDebugStringA(buf);
#else
	UNREFERENCED_PARAMETER(pFormat);
#endif // _DEBUG || DEBUG
}

#endif // _DEBUG_PRINTF_H_