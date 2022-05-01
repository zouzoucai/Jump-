/*==============================================================================

	フェード処理　[Fade.h]

													Author : Sai Hakusen
													Date   : 2020 / 01 / 05
==============================================================================*/
#ifndef FADE_H_
#define FADE_H_

void Fade_Initialize(void);
void Fade_Finalize(void);
void Fade_Update(void);
void Fade_Draw(void);

typedef enum FadeMode_tag
{
	FADE_NONE,
	FADE_IN,
	FADE_OUT,
}FadeMode;

void Fade_Start(FadeMode mode, float r, float g, float b, int frame);
bool Fade_IsFade(void);

#endif // FADE_H_