# キャッシュの管理プログラム

# 目的
最も直近にアクセスされた上位X個の<URL, Webページ>の組が保存できるデータ構造
## 必要な仕様
* 与えられたURLがキャッシュデータにあるかないかを検索する
* 既にある場合
	* アクセス順のデータの最後に移動させる
* ない場合
	* キャッシュデータに内で一番古い<URL, Webページ>を捨てる
	* キャッシュデータに与えられた<URL, Webページ>を追加する
	* アクセス順のデータの最後に追加する

# 概要
![structure](https://github.com/ayumiohno/step/tree/main/02_data_structure1/problem3and4/structure.jpg)