#include "point.hpp"
#include "tsp.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    std::vector<Point> points; //点の座標を保存するvector

    {
        std::cerr << "put file number" << std::endl;
        std::string file_num;
        std::cin >> file_num;
        auto file_name = "../google-step-tsp/input_" + file_num + ".csv";
        std::ifstream file(file_name);
        std::string data;
        while (std::getline(file, data)) {
            auto index = data.find(',');
            auto x = data.substr(0, index);
            auto y = data.substr(index + 1, data.size() - x.size() - 1);
            try {
                points.push_back(Point{std::stod(x), std::stod(y)});
            } catch (const std::invalid_argument& e) {
                std::cerr << e.what() << std::endl;
            }
        }
        file.close();
    }

    DpMemo dp_memo{(int)points.size()};

    /*2点を結ぶだけの場合で初期化. 通過点vectorは{}*/
    for (int end_id = 1; end_id < points.size(); ++end_id) {
        dp_memo.pushMinDistance(end_id, {}, (points.at(0) - points.at(end_id)).getNolm());
    }

    /*始点と終点の間の通過点を1点から(点の合計-2)点まで変化させる*/
    for (int using_num_of_dots = 1; using_num_of_dots <= points.size() - 2; ++using_num_of_dots) {
        increaseNumOfDot(dp_memo, using_num_of_dots, points);
    }

    /*始点と終点を0で一致させて最後の更新*/
    finalDpMemo(dp_memo, points);

    /*最後の更新時の通過点(0以外のすべての点)*/
    std::vector<int> passing_points_final;
    for(int i = 1; i < points.size(); ++ i){
        passing_points_final.push_back(i);
    }

    std::cout << dp_memo.getMinDistance(0, passing_points_final) << std::endl;
}
