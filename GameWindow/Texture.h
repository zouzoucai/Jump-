/*==============================================================================

	テクスチャ処理　[Texture.h]

													Author : Sai Hakusen
													Date   : 2020 / 12 / 15
==============================================================================*/
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <d3d9.h>

#define TEXTURE_FILENAME_MAX (64)	// テクスチャファイル名の最大文字数
#define TEXTURE_INVALID_ID (-1)		// 無効なテクスチャ管理番号

// 初期化の引数に動的メモリ確保で管理最大数とか入れると良い
void Texture_Initialize(void);
// 基本使わないけど動的メモリ確保やるならここで後片付け
void Texture_Finalize(void);

// テクスチャファイルの読み込み予約
// 引数 pFileName...ファイル名
// 戻り値 テクスチャ管理番号...最大管理数を超えていた場合はINVALID_TEXTURE_ID
int Texture_SetTextureLoadFile(const char* pFileName);

// テクスチャの読み込み
// 予約されているファイルを読み込みます
// 戻り値 読み込めなかったファイル数または他の不具合が発生した場合-1
// 
int Texture_Load(void);

// int textureIds[] => int* pTextureIds
// テクスチャの部分開放(メモリをあまり食わないようにする)
// 引数 textureIds[]...開放したいtextureIdが入ったint型配列の先頭アドレス
//      count...開放するテクスチャの数
void Texture_Release(int textureIds[], int count);

// テクスチャの全開放
void Texture_AllRelease(void);

unsigned long Texture_GetTextureWidth(int textureId);
unsigned long Texture_GetTextureHeight(int textureId);

// テクスチャインタフェースポインタの取得
// 読み込めていないまたは不正な管理番号だった場合はNULL
LPDIRECT3DTEXTURE9 Texture_GetTexture(int textureId);



#endif // TEXTURE_H_