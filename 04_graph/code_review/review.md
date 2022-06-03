# 実行
* wslでも両方動きました！
# 全体的に
* 分かりやすく書けていていて凄いと思います。
    * 変数の説明をまとめて入れているところ
    * ブロックごとの改行の仕方
    * が特に分かりやすかったです
* bfsとdfsで共通の部分を括りだすと、もっとスッキリすると思いました。
* 読み込んだファイルデータを収納するデータ構造が同じなので、読み込んだ後に両方テストできるようにした方が、デバックが速いか思います。
``` cpp
map<string, string> pages;
map<int, set<int>> links;
```
* また、bfs(), dfs()もほとんど同じなので、stack, queueをテンプレート引数にすれば、面白いかと思いました。
    * ただ、これは好みにもよる気がします。
    * 大雑把には下のイメージです。(listはstd::listではないです)
``` cpp
template<class T>
fs(T& list, const std::function<int(T&)>&& get_first)
{

}
...
int main()
{
    std::stack<int> list;
    fs<std::stack<int>>(list, [](std::stack<int>& list){return list.top();});
}
```
# ファイル読み込み
* サンプルコードの範囲ですが、pagesとlinksの読み込みの共通部分をモジュール化できると便利だと思います。
# FS
* 変数名のつけ方などが分かりやすいです。
* 追記します！
