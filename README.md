# GL-Server

1. VS-Communityをインストする
2. 「プロジェクト」→「NuGetパッケージ管理」  
「参照」でnupenglを検索。nupenglをインスト
3. https://qiita.com/tositada_nakada/items/1528c7dad40f33b5e31b
をみてGLをインストする  
 「構成」＝「デバッグ」→「…のプロパティ」から設定
4. リンカー設定は以下追加  
glew32s.lib glfw3.lib glu32.lib opengl32.lib ws2_32.lib
5. 「C++」→「詳細設定」→「指定の警告を無効にする」
6. WSL  
設定⇒アプリ⇒プログラム機能設定
