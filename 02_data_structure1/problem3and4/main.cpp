#include "cache.hpp"
//#include "data.hpp"

int main()
{

    Data p1{"https://a.com", "aaa"};
    Data p2{"https://b.ac.jp", "bbb"};
    Data p3{"https://www.go.jp", "bbb"};

    Cache cache{};

    cache.processData(&p1);
    cache.processData(&p2);
    cache.processData(&p2);

    //url入手
    //cacheのhash table(array)のうちどこに所属するか
    //arrayの中のリストにあるかどうか
    //あったとき
    //入った順番を管理するリストの最後に移動する(消してaddする)
    //なかったとき
    //page classをつくる
    //cacheのarrayの中のリストに加える
    //入った順番を管理するリストの最後に追加する
    //両方のリストから抜ける

    return 0;
}
