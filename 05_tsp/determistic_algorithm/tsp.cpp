#include "point.hpp"
#include "tsp.hpp"

/* passing_points : 通過点のidのvector, end_id : 最終点のid
   points : Pointsクラスのvector
   dp_memo : 一つ前に更新したのDpMemo, dp_memo_new : 今回更新しようとしているdp_memo*/
void updateDpMemo(const std::vector<int>& passing_points,
    const int& end_id, const std::vector<Point>& points,
    DpMemo& dp_memo, DpMemo& dp_memo_new)
{
    double min_length = 1 << 30;
    /*最終点の一つ前を通過点の中で動かしていく*/
    for (int i = 0; i < passing_points.size(); ++i) {
        auto v = passing_points;
        v.erase(v.begin() + i);
        /*length = 始点~最終点の一つ前の距離(DpMemoを参照) + 最終点の一つ前~最終点の距離(直線)*/
        double length = dp_memo.getMinDistance(passing_points.at(i), v)
                        + (points.at(passing_points.at(i)) - points.at(end_id)).getNolm();
        min_length = std::min(min_length, length);
    }
    dp_memo_new.pushMinDistance(end_id, passing_points, min_length);
}

/* next_id : 次に通過点に挿入する点のid */
void setPassingPointsLoop(int next_id, std::vector<int> passing_points,
    const int& using_num_of_dots, const int& end_id,
    const std::vector<Point>& points,
    DpMemo& dp_memo, DpMemo& dp_memo_new)
{
    passing_points.push_back(next_id);
    if (passing_points.size() == using_num_of_dots) {  //通過点追加完了
        updateDpMemo(passing_points, end_id, points, dp_memo, dp_memo_new);
        return;
    }
    for (int i = next_id + 1; i < points.size(); ++i) {  //最終点以外で通過点を入れていく
        if (i == end_id) {
            continue;
        }
        setPassingPointsLoop(i, passing_points, using_num_of_dots, end_id, points, dp_memo, dp_memo_new);
    }
}

/* using_num_of_dots : 今回通過点に入れる点の数 */
void increaseNumOfDot(DpMemo& dp_memo, int& using_num_of_dots, const std::vector<Point>& points)
{
    std::cerr << "increase Num: " << using_num_of_dots << std::endl;
    DpMemo dp_memo_new{(int)points.size()};
    /*最終点を動かしていく*/
    for (int end_id = 1; end_id < points.size(); ++end_id) {
        /*始点0の次の点を動かしていく*/
        for (int starting_id = 1; starting_id < points.size(); ++starting_id) {
            if (starting_id == end_id) {
                continue;
            }
            setPassingPointsLoop(starting_id, {}, using_num_of_dots, end_id, points, dp_memo, dp_memo_new);
        }
    }
    dp_memo = std::move(dp_memo_new);
}

void finalDpMemo(DpMemo& dp_memo, const std::vector<Point>& points)
{
    std::cerr << "increase Num: final" << std::endl;
    DpMemo dp_memo_new{1};  //end_idは一つだけなのでsizeは1
    /*始点0の次の点を動かしていく*/
    for (int starting_id = 1; starting_id < points.size(); ++starting_id) {
        setPassingPointsLoop(starting_id, {}, points.size() - 1, 0, points, dp_memo, dp_memo_new);
    }
    dp_memo = std::move(dp_memo_new);
}
