#include "cache.hpp"
#include <memory>
//#include "data.hpp"

int main()
{

    /*auto p1 = std::make_shared<Data>("https://a.com", "aaa");
    auto p2 = std::make_shared<Data>("https://b.ac.jp", "bbb");
    auto p3 = std::make_shared<Data>("https://www.go.jp", "ccc");
    auto p4 = std::make_shared<Data>("https://www.go.jp", "ccc");*/
    auto url1 = "https://a.com";
    auto url2 = "https://b.ac.jp";
    auto url3 = "https://www.go.jp";

    Cache cache{};

    cache.processData(url1);
    cache.processData(url2);
    cache.processData(url3);
    cache.processData(url3);

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
