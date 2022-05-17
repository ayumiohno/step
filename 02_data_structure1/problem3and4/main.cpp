#include "cache.hpp"

int main()
{
    auto url1 = "https://a.com";
    auto url2 = "https://b.ac.jp";
    auto url3 = "https://www.go.jp";
    auto url4 = "https://abcdefg.com";
    auto url5 = "https://utxyz.go.jp";
    auto url6 = "https://ooo.go.jp";
    auto url7 = "http://itc-lms.ecc.u-tokyo.ac.jp";

    Cache<23, 5> cache{};

    cache.processData(url1);
    cache.processData(url2);
    cache.processData(url3);
    cache.processData(url2);
    cache.processData(url4);
    cache.processData(url5);
    cache.processData(url6);
    cache.processData(url1);
    cache.processData(url2);
    cache.processData(url7);

    return 0;
}
