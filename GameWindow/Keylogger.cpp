/*==============================================================================

	キー入力＆キー入力の記録処理　[Keylogger.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 12 / 25
==============================================================================*/
#include "Keylogger.h"
#include "keyboard.h"

typedef unsigned long KeyloggerType;

static  KeyloggerType g_CurrentData = 0;
static  KeyloggerType g_PrevData = 0;
static  KeyloggerType g_TriggerData = 0;
static  KeyloggerType g_ReleaseData = 0;

static int g_KeyloggerMode = 0;	// 0がノーマル、1が記録、2が再生
static KeyloggerType* g_pRecordCurrentData = NULL;
static int g_RecordFrame = 0;

static int g_RecordPlayFrame = 0;
static int g_RecordFrameMax = 0;	// 記録上限

// キーボードとキーロガーの変換表
static Keyboard_Keys g_KKs[KL_MAX] = {
	KK_W,
	KK_S,
	KK_A,
	KK_D,
	KK_SPACE,
	KK_B,
	KK_ENTER,
	KK_M,
	KK_ESCAPE,
};

void Keylogger_Initialize(void)
{
	g_CurrentData = g_PrevData = g_TriggerData = g_ReleaseData = 0;
	g_KeyloggerMode = 0;
	g_pRecordCurrentData = NULL;
	g_RecordFrame = 0;
	g_RecordPlayFrame = 0;
	g_RecordFrameMax = 0;
}

void Keylogger_Finalize(void)
{
	if (g_pRecordCurrentData) {
		free(g_pRecordCurrentData);
		g_pRecordCurrentData = NULL;
	}
}

void Keylogger_Update(void)
{
	g_PrevData = g_CurrentData;

	// キーロガーモードによって動作変更
	if (g_KeyloggerMode <= 1) {
		g_CurrentData = 0;

		for (int i = 0; i < KL_MAX; i++) {
			if (Keyboard_IsKeyDown(g_KKs[i])) {
				g_CurrentData |= 1u << i;
			}
		}
		if (g_KeyloggerMode == 1) {
			//if(g_RecordFrame < マックス超えたら記録を止めてロガーモードにする)
			if (g_RecordFrame < g_RecordFrameMax) {
				g_pRecordCurrentData[g_RecordFrame] = g_CurrentData;
				g_RecordFrame++;
			}
		}
	}
	else {
		// 安全処置（記録フレーム数よりも多く再生しようとしたら）
		if (g_RecordPlayFrame < g_RecordFrame) {
			g_CurrentData = g_pRecordCurrentData[g_RecordPlayFrame++];
		}
		else {
			g_CurrentData = 0;
			g_KeyloggerMode = 0;	//	再生が終わったので普通モードに戻す
		}
	}

	g_TriggerData = (g_PrevData ^ g_CurrentData) & g_CurrentData;
	g_ReleaseData = (g_PrevData ^ g_CurrentData) & g_PrevData;
}


// 押してたら
bool Keylogger_Press(KeyKind kl)
{
	return g_CurrentData & (1u << (int)kl);
}

// 押した瞬間
bool Keylogger_Trigger(KeyKind kl)
{
	return g_TriggerData & (1u << (int)kl);
}

// 離した瞬間
bool Keylogger_Release(KeyKind kl)
{
	return g_ReleaseData & (1u << (int)kl);
}

void Keylogger_RecordStart(int frame_max)
{
	if (g_pRecordCurrentData) {
		free(g_pRecordCurrentData);
	}

	g_pRecordCurrentData = (KeyloggerType*)malloc(sizeof(unsigned char) * frame_max);
	g_KeyloggerMode = 1;
	g_RecordFrame = 0;

	g_RecordFrameMax = frame_max;
}

void Keylogger_RecordEnd(void)
{
	if (!g_pRecordCurrentData) {
		return;
	}

	FILE* fp = fopen("keylogger.dat", "wb");
	// ファイルの先頭にデータ量を記録
	fwrite(&g_RecordFrame, sizeof(g_RecordFrame), 1, fp);
	// ファイルへデータを保存
	fwrite(g_pRecordCurrentData, sizeof(unsigned char), g_RecordFrame, fp);
	fclose(fp);

	g_KeyloggerMode = 0;

	if (g_pRecordCurrentData) {
		free(g_pRecordCurrentData);
		g_pRecordCurrentData = NULL;
	}
}

void Keylogger_RecordLoad(void)
{
	FILE* fp = fopen("keylogger.dat", "rb");
	fread(&g_RecordFrame, sizeof(g_RecordFrame), 1, fp);

	if (g_pRecordCurrentData) {
		free(g_pRecordCurrentData);
	}
	g_pRecordCurrentData = (KeyloggerType*)malloc(sizeof(unsigned char) * g_RecordFrame);
	fread(g_pRecordCurrentData, sizeof(unsigned char), g_RecordFrame, fp);
	fclose(fp);
}

void Keylogger_RecordPlay(void)
{
	g_RecordPlayFrame = 0;
	g_KeyloggerMode = 2;
}

bool Keylogger_IsRecordPlay(void)
{
	if (g_KeyloggerMode == 2) {
		return true;
	}
	return false;
}