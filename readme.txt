■作品名：ぐるねじ

■製作時間：2ヶ月

■実行環境
Visual Studio 
Debug / Release
x64

■主に画面遷移マシンや各自画面内の処理が担当しています。
Fade.cpp/.h
Game.cpp/.h
Game_Bg.cpp/.h
Game_Goal.cpp/.h
Game_Pasue.cpp/.h
Menu.cpp/.h
my_font.cpp
Result.cpp/.h
Scene.cpp/.h
Sprite.cpp/.h
Title.cpp/.h

■使用したライブラリー：DX9

■アピールポイント：Scene.cppで画面遷移用のマシンを構築しました。

■操作方法
【メニューなどのゲーム以外の部分】
▼キーボード
WASDキー　…カーソル移動
Spaceキー …決定
Bキー　　 …キャンセル

▼Xbox（ロジクールXInputモード）
方向ボタン/スティック…カーソル移動
Bボタン　　　　　　　…決定
Aボタン　　　　　　　…キャンセル

▼DUALSHOCK4（ロジクールDirectInputモード）
方向ボタン/スティック…カーソル移動
○ボタン　　　　　　 …決定
×ボタン　　　　　　 …キャンセル

【ゲーム中】
▼キーボード
地上
A/Dキー　　 　…左右に移動
Spaceキー　 　…ジャンプ

空中
A/Dキー　 　　…左右に移動
ネジに近づく　…ドライバーを突き刺す

突き刺し状態
A/Dキー　　　…左右に回転
Spaceキー　　…遠心力で飛ぶ

常時
Mキー　　　　…ポーズ

ポーズ中
Mキー/Bキー　…ポーズ解除
Spaceキー　　…決定

▼Xbox（ロジクールXInputモード）
左右ボタン/左スティック　…移動
（回転中） 左スティック  …ジャンプ方向調整
Bボタン　　　　　　　　　…ジャンプ/決定
LT/RTボタン　　　　　　　…回転
STARTボタン　　　　　　　…ポーズ/ポーズ解除
Aボタン			 …ポーズ解除
BACKボタン　　　　　　　 …リセット

▼DUALSHOCK4（ロジクールDirectInputモード）
左右ボタン/左スティック　…移動
（回転中） 左スティック  …ジャンプ方向調整
○ボタン　　　　　　　　 …ジャンプ/決定
L2/R2ボタン　　　　　　　…回転
OPTIONSボタン　　　　　　…ポーズ/ポーズ解除
×ボタン		 …ポーズ解除
SHAREボタン/パッドボタン …リセット
