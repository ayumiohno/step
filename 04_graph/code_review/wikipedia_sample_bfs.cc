#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <queue>
#include <stack>


using namespace std; // 追加, std::を省略しました

const bool debug = false;
const int MAX_ID = 10000000;


map<string, string> pages;
map<int, set<int>> links; // int型に変更しました

queue<int> q; //ノードを入れるキュー
queue<int> q_num_step_count; // ノードに来るまでに何ステップかかるか
vector<bool> visited(MAX_ID); // 訪ねるノードをマーク
vector<int> prev_node(MAX_ID,-1); //　prev(id):idの前に訪れたノードのid

int bfs(int id_node, int id_start, int id_target, int num_step_count) {
    /*

    id_node : 現在のノードのID
    id_start : スタート地点のノードのID
    id_target : 目的地のノードのID
    num_step_count : 目的地までたどり着くのにかかるステップ数
    
    関数の返り値 : 
    id_startからid_targetにたどり着ける場合は、何回でたどり着けるかを返す
    たどり着けない場合は、-1を返す

    */

    q.push(id_start);
    q_num_step_count.push(0);
    
    while (!q.empty()) {

        id_node = q.front(); //これから調べるノードID(int)
        q.pop();

        num_step_count = q_num_step_count.front(); // id_nodeにたどり着くまでに何回かかるか
        q_num_step_count.pop();

        if (debug) { // デバッグ出力
            cout << "bfs " << "id_node:" << id_node << ":" << pages[to_string(id_node)] << " id_start:" << id_start << "id_target:" << id_target << "stepcount:" << num_step_count << endl;
        }


        if (id_node == id_target) { // 発見
            return num_step_count; // ステップ数を返して終了
        }


        if (links.count(id_node) > 0) {

            set<int> set_id = links[id_node];
            for (auto itr = set_id.begin(); itr != set_id.end(); ++itr) {
                int id_next = *itr;
                if (!visited.at(id_next)) { 
                    visited.at(id_next) = true; // ノードをマーク
                    if (id_node != id_next) {
                        q.push(id_next); // 次に訪れるノードをキューにプッシュ
                        q_num_step_count.push(num_step_count + 1); // 何回かかったか
                        prev_node.at(id_next) = id_node; // 経路をおぼえておくために、ひとつ前のノードIDを保存
                    }
                }
            }
        }
    }

    return -1;


}


int main() {

    int num_max_id = 0;

  {
    ifstream file("data/pages.txt");
    string data;
    while (getline(file, data)) {
  
      auto index = data.find('\t');
      string id = data.substr(0, index);
      string title = data.substr(index + 1, data.size() - id.size() - 1);

      pages[(id)] = title;

      if (stoi(id) > num_max_id)num_max_id = stoi(id);
    }
  }

  {
    ifstream file("data/links.txt");
    string data;
    while (getline(file, data)) {
      auto index = data.find('\t');
      auto from = data.substr(0, index);
      auto to = data.substr(index + 1, data.size() - from.size() - 1);
     
      links[stoi(from)].insert(stoi(to));
      
    }
  }

  int id_start=-1;
  int id_target=-1;

  for (const auto& page : pages) {
    if (page.second == "Google") {
        id_start = stoi(page.first);
    }
    if (page.second == "渋谷"){//　ターゲットノード
        id_target = stoi(page.first);
    }
  }


  cout << "max id:" << num_max_id << endl;
  //bfs関数で計算します。
  int ans = bfs(id_start, id_start, id_target, 0);

  // 何回でたどり着けるかを出力します。たどり着けない場合は-1を出力します。
  cout << "ans : " << ans << endl; 

  // 経路を出力します
  int id = id_target;
  while (id != -1) {
      cout << "id : " << id << " " << pages[to_string(id)] << endl;
      id = prev_node.at(id);
  }

  return 0;
}