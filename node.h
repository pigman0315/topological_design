#include <iostream>
#include <math.h>
using namespace std;

extern const float SPEED;

class Node{
public:
	float x;
	float y;
public:
	Node(){
		x = -1;
		y = -1;
	}
	Node(int nx, int ny){
		x = nx;
		y = ny;
	}
	bool operator== (const Node& n) const{
		return (x == n.x && y == n.y);
	}
	bool operator< (const Node& n) const{
		// use Euclidean distance to origin point to determine which node is bigger
    	return (x*x+y*y) < (n.x*n.x+n.y*n.y);
	}
	void show(){
		cout << x << " " << y << endl;
	}
};
class SavingsNode{
public:
	float value;
	int i,j;
	Node ni;
	Node nj;
public:
	SavingsNode(){
		value = -1;
	}
	SavingsNode(float v, int i_, int j_, Node n1, Node n2){
		value = v;
		i = i_;
		j = j_;
		ni = n1;
		nj = n2;
	}
	void show(){
		cout << "Saving value: " << value << endl;
		cout << "i: " << i << ", j: " << j << endl;
		ni.show();
		nj.show(); 
	}
};
class SolutionNode{
public:
	vector< vector<int> > routes_table;// be used to record routes with customer index of customer_points
	vector<float> routes_time;
	float total_time;
	int route_num;
public:
	SolutionNode(){}
	SolutionNode(vector< vector<int> > _routes_table, 
				 vector<Node> customer_points,
				 Node exch_point,
				 vector< vector<float> > dist_table)
	{
		routes_table = _routes_table;
		route_num = routes_table.size();
		total_time = 0.0;
		for(int i = 0;i < routes_table.size();i++){
			float dist;
			float time = 0.0;
			for(int j = 0;j < routes_table[i].size();j++){
				if(j == 0){
					int cur_n = routes_table[i][j];
					dist = dist_table[cur_n][cur_n]; // distance from cur_n to exchange point
					// dist = sqrt((cur_n.x - exch_point.x)*(cur_n.x - exch_point.x) + (cur_n.y - exch_point.y)*(cur_n.y - exch_point.y));
				}
				else{
					int cur_n = routes_table[i][j];
					int prev_n = routes_table[i][j-1];
					// dist = sqrt((cur_n.x - prev_n.x)*(cur_n.x - prev_n.x) + (cur_n.y - prev_n.y)*(cur_n.y - prev_n.y));
					dist = dist_table[cur_n][prev_n]; // distance from cur_n to prev_n
				}
				time += dist / SPEED;
			}
			int last_n = routes_table[i][routes_table[i].size()-1];
			// dist = sqrt((last_n.x - exch_point.x)*(last_n.x - exch_point.x) + (last_n.y - exch_point.y)*(last_n.y - exch_point.y));
			dist = dist_table[last_n][last_n]; // distance from last_n to exchange point
			time += dist / SPEED;
			total_time += time;
			routes_time.push_back(time);
		}
	}
	SolutionNode(vector< vector<int> > _routes_table,vector<float> _routes_time){
		routes_table = _routes_table;
		route_num = routes_table.size();
		total_time = 0.0;
		for(int i = 0;i < route_num;i++){
			total_time += _routes_time[i];
		}
	}
	SolutionNode(vector< vector<int> > _routes_table,
				 vector<bool> _routes_flg,
				 vector<float> _routes_time,
				 int customer_num)
	{
		route_num = 0;
		total_time = 0.0;
		for(int i = 0;i < customer_num;i++){
			if(_routes_flg[i] == true){
				route_num++;
				vector<int> tmp_v;
				int rn = _routes_table[i].size();
				for(int j = 0;j < _routes_table[i].size();j++){
					tmp_v.push_back(_routes_table[i][j]);
				}
				total_time += _routes_time[i];
				routes_time.push_back(_routes_time[i]);
				routes_table.push_back(tmp_v);
			}
		}
	}
	int get_node_num(){
		int cnt = 0;
		for(int i = 0;i < routes_table.size();i++){
			for(int j = 0;j < routes_table[i].size();j++){
				cnt++;
			}
		}
		return cnt;
	}
	void show(){
		int cnt = 0;
		for(int i = 0;i < routes_table.size();i++){
			for(int j = 0;j < routes_table[i].size();j++){
				cnt++;
				cout << routes_table[i][j] << " ";
			}
			cout << ", time: " << routes_time[i] << endl;
		}
		cout << "Total node number: " << cnt << endl;
		cout << "Total time: " << total_time << endl;
	}
};