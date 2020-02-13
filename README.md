# GL-Server

1. VS-Communityをインストする
2. https://qiita.com/tositada_nakada/items/1528c7dad40f33b5e31b
をみてGLをインストする  
 「構成」＝「デバッグ」→「…のプロパティ」から設定
3. リンカー設定は以下追加  
glew32s.lib glfw3.lib glu32.lib opengl32.lib ws2_32.lib
4. 「C++」→「詳細設定」→「指定の警告を無効にする」
5. WSL  
設定⇒アプリ⇒プログラム機能設定
