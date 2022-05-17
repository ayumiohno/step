キャッシュの管理プログラム
=

# 目的
* 最も直近にアクセスされた上位X個の<URL, Webページ>の組が保存できるデータ構造
* 各操作をO(1)で行う
## 必要な仕様
* 与えられたURLがキャッシュデータにあるかないかを検索する
* 既にある場合
	* アクセス順のデータの最後に移動させる
* ない場合
	* キャッシュデータに内で一番古い<URL, Webページ>を捨てる
	* キャッシュデータに与えられた<URL, Webページ>を追加する
	* アクセス順のデータの最後に追加する

# 概要
![structure](https://github.com/ayumiohno/step/tree/main/02_data_structure1/problem3and4/structure.png?raw=true)
## Data @data.hpp
* url, contentの管理用
* urlごとに作られる
## List @list.hpp, list.cpp
* 双方向リストにおける最初のNodeと最後のNodeとtagを保持
* 二種のインスタンスが作られる
	* hash tableの各エントリリスト用(HASH tagを持つ)
	* hash アクセス順の管理用(ORDER tagを持つ)
## Node @node.hpp
* dataと、node間の前後関係を表すUnitから成る
* Unitは以下を管理
	* left : 左側のnode
	* right : 右側のnode
	* list : どの連結リスト上での前後関係か
* 各NodeはHashとOrderのunitを一つずつ持つ
## DataBase @ data_base.hpp
* urlとcontentの一覧があり、urlからcontentをロードできる
	* キャッシュのデータ構造とは直接関係ないのでO(1)でないことに注意
* キャッシュデータに存在しないurlのcontentの取得に用いる
## Cache @cache.hpp
* hash_table, order_list, databaseを持つ
* 初期化時にhash_tableの各hash_listとorder_list、databaseのインスタンスを作成
* urlを得たときに、hash_listやorder_listの操作、新たなdataとnodeのインスタンスの作成を行う
* hash tableの行数 TABLE_SIZE と、キャッシュに保存できるデータ数 MEMORY_SIZEをtemplateで指定できる。
## main.cpp
* Cacheのインスタンスを作成
* cacheにurlを渡す

# 工夫した点
* 各nodeを二方向の双方向リストと関連付けることによって、以下のことが可能になったこと
1. 要素の削除と追加がO(1)で容易にできること
	* 削除する要素の前後のnodeのleft, right
	* listでfirst, lastだった場合はfirstかlast
	* を変更するだけで済む
	* 要素数が大きいorder_listに対しては大きな利点となる。
2. キャッシュからデータを削除する際に、他の要素のアドレスの変更がないこと
	* もしvectorで管理すると、途中の要素を削除するとそれ以降の要素のアドレスが一つずつずれてしまう
	* hash_listで双方向リストを使用したのはこのためである
3. order_listの探索をほぼO(1)でできること
	* 通常、双方向リストでの探索にはO(n)必要だが、hash_listを経由してorder_listの探索を行うことで、ほぼO(1)でできる
	* キャッシュにあるurlが再度登場した際のorder_listの更新時に対象要素の探索が必要であり、その際の計算量を押さえられることが、nodeと二方向のリストを関連付けた理由である
* List↔Node↔Node間の相互依存関係における循環参照を防いだこと
	* shared_ptrを用いたが、通常shared_ptrでは循環参照によって、双方のポインタの解放が行われなくなる危険がある
	* 今回は、不要になったNodeの削除の際に、自身を参照している他のList, Nodeの参照先を、他のNode(あるいはnullptr)にすることによって、自身の解放を行えるようにした
# 実行結果
* result.txtに実行結果を記録した
* 入力はすべてmain.cppで直接行った
* 入力したurlとcontentのほか、新たにデータを追加した場合は"create new data"、既にキャッシュにあった場合は"url already exist"と出力される
## 入力
* hash_table：23行
* 保存するdata数 X : 5個
* 入力順\
url1(#15) -> url2(#9) -> url3(#12) -> url2 (#9)-> url4(#4) -> url5(#12) -> url6(#12) -> url1(#15) -> url2(#9) -> url7(#14)
## 結果
* url1 : create new data
* url2 : create new data
* url3 : create new data
* url2 : url already exist
* url4 : create new data
* url5 : create new data
* url6 : create new data
* url1 : create new data
* url2 : url already exist
* url7 : create new data

