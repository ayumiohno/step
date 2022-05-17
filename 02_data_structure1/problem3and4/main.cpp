#include "cache.hpp"
#include <memory>
//#include "data.hpp"

auto generateNode(std::string url)
{
    return std::make_shared<Data>(url, "aaa");
}

int main()
{

    auto url1 = "https://a.com";
    auto url2 = "https://b.ac.jp";
    auto url3 = "https://www.go.jp";
    auto url4 = "https://abcdefg.com";
    auto url5 = "https://utxyz.go.jp";
    auto url6 = "https://xyzut.go.jp";

    Cache cache{};

    cache.processData(url1);
    cache.processData(url2);
    cache.processData(url3);
    cache.processData(url2);
    cache.processData(url4);
    cache.processData(url5);
    cache.processData(url6);
    cache.processData(url1);

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
