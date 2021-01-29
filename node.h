#include <iostream>
using namespace std;
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
	map<int,int> routes_map; // be used to map customer with his route number
	vector<float> routes_time;
	float total_time;
	int route_num;
public:
	SolutionNode(){}
	SolutionNode(vector< vector<int> > _routes_table,
				 map<int,int> _routes_map,
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
					routes_map[_routes_table[i][j]] = rn;
				}
				routes_time.push_back(_routes_time[i]);
				total_time += _routes_time[i];
				routes_table.push_back(tmp_v);
			}
		}
	}
	void show(){
		int cnt = 0;
		for(int i = 0;i < route_num;i++){
			for(int j = 0;j < routes_table[i].size();j++){
				cnt++;
				cout << routes_table[i][j] << " ";
			}
			cout << ",time: " << routes_time[i] << endl;
		}
		cout << "Total node number: " << cnt << endl;
		cout << "Total time: " << total_time << endl;
	}
};