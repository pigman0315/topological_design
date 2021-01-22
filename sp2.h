#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <random>
#include "node.h" // class Node/SavingsNode
//
extern int w, m_I, m_O, best_rot_deg;
extern Node best_1st_center;
extern vector<Node> best_2nd_center;
extern vector< vector<Node> > best_3rd_center;
extern vector< vector<Node> > district_endpoints_1st;
extern vector< vector< vector<Node> > > district_endpoints_2nd;
extern vector< vector<Node> > district_customers_1st;
extern vector< vector< vector<Node> > > district_customers_2nd;
extern vector<Node> exch_points_1st;
extern vector< vector<Node> > exch_points_2nd;
extern float T;
extern const float SPEED;
//
// randomized savings algo.
//
class SavingsAlgo{
public:
	// variables
	vector< vector<int> > routes_table;// be used to record routes with customer index of customer_points
	map<int,int> routes_map; // be used to map customer with his route number
	vector<bool> routes_flg; // to show which routes in map is available
	vector<float> routes_time; // store each routes' time
	vector<SavingsNode> savings_list;
	vector<Node> customer_points;
	int customer_num;
	Node exch_point;
	static const int RSA_P = 10; // parameter for randomized savings algo.
	static const int RSA_Q = 10; // parameter for randomized savings algo.
	static const int RSA_U = 3; // parameter in minimize_routes to optimize randomized savings algo.
	// functions
	SavingsAlgo(vector<Node> cps, Node ep);
	void run();
	void get_savings();
	float get_dist(Node n1, Node n2);
	bool check_sn(SavingsNode sn);
	void delete_maxP_sn(int max_p);
	SavingsNode find_valid_sn();
	void merge_route(int sn_i, int sn_j);
	void do_savings_algo();
	void show_routes();
	void minimize_routes();
	void get_routes_time();
	bool insert_node(Node n_insert, int n_insert_num);
};
// constructor
SavingsAlgo::SavingsAlgo(vector<Node> cps, Node ep){
	customer_points = cps;
	exch_point = ep;
	customer_num = customer_points.size();
	// make route table with each customer node
	for(int i = 0;i < customer_num;i++){
		vector<int> tmp;
		tmp.push_back(i);
		routes_table.push_back(tmp);

		// register customer node number into route map
		routes_map[i] = i;

		// set routes flg
		routes_flg.push_back(true);

		// reset routes time
		routes_time.push_back(0.0);
	}
}
// funciton
void SavingsAlgo::run(){
	// calculate savings value for each pair of node
	get_savings();

	// do randomized savings algo to find initial routes
	do_savings_algo();


	// get new routes time after updating
	get_routes_time();
	// show
	show_routes();

	cout << "-----" << endl;

	// try to minimize the number of routes
	minimize_routes();

	// get new routes time after updating
	get_routes_time();

	// show routes
	show_routes();
}
void SavingsAlgo::get_routes_time(){
	for(int i = 0;i < customer_num;i++){
		float sum = 0.0;
		if(routes_flg[i] == true){
			sum += get_dist(exch_point,customer_points[routes_table[i][0]]);
			for(int j = 0;j < routes_table[i].size();j++){
				if(j < routes_table[i].size()-1)
					sum += get_dist(customer_points[routes_table[i][j]],customer_points[routes_table[i][j+1]]);
			}
			sum += get_dist(customer_points[routes_table[i][routes_table[i].size()-1]],exch_point);
			sum = sum / SPEED;
		}
		routes_time[i] = sum;
	}
}
bool SavingsAlgo::insert_node(Node n_insert, int n_insert_num){
	for(int i = 0;i < customer_num;i++){
		if(routes_flg[i] == true){
			for(int j = 0;j <= routes_table[i].size();j++){
				float add_dist = 0.0;
				float add_time = 0.0;
				if(j == routes_table[i].size()){
					Node n_last = customer_points[routes_table[i][routes_table[i].size()-1]];
					add_dist = get_dist(exch_point,n_insert)+get_dist(n_insert,n_last)-get_dist(exch_point,n_last);
				}
				else{
					Node n_cur = customer_points[routes_table[i][j]]; // current node in routes
					if(j == 0 || j == routes_table[i].size()){
						add_dist = get_dist(exch_point,n_insert)+get_dist(n_insert,n_cur)-get_dist(exch_point,n_cur);
					}
					else{
						Node n_prev = customer_points[routes_table[i][j-1]];
						add_dist = get_dist(n_insert,n_prev)+get_dist(n_insert,n_cur)-get_dist(n_prev,n_cur);
					}
				}
				add_time = add_dist / SPEED;
				if(add_time + routes_time[i] <= T){
					// do insert
					routes_table[i].insert(routes_table[i].begin()+j,n_insert_num);
					routes_time[i] += add_time;
					routes_map[n_insert_num] = i;
					return true;
				}	
			}
		}
	}
	return false;
}
void SavingsAlgo::minimize_routes(){
	vector<int> single_node_nums;
	// release routes
	for(int i = 0;i < routes_table.size();i++){
		if(routes_table[i].size() <= RSA_U && routes_flg[i] == true){
			for(int j = 0;j < routes_table[i].size();j++){
				routes_table[routes_table[i][j]].clear();
				routes_table[routes_table[i][j]].push_back(routes_table[i][j]);
				single_node_nums.push_back(routes_table[i][j]);
			}
			routes_table[i].clear();
			routes_table[i].push_back(i);
			routes_flg[i] = false;
			
		}
	}
	cout << single_node_nums.size() << endl;
	// insert those single node into current available routes
	for(int n = 0;n < single_node_nums.size();n++){
		cout << single_node_nums[n] << endl;
		Node n_insert = customer_points[single_node_nums[n]]; // node we want to insert 
		if(!insert_node(n_insert, single_node_nums[n])){
			cout << "insert node " << single_node_nums[n] << " failed." << endl;
		}
	}
	
	
}
void SavingsAlgo::show_routes(){
	for(int i = 0;i < customer_num;i++){
		if(routes_flg[i] == true){
			for(int j = 0;j < routes_table[i].size();j++){
				cout << routes_table[i][j] << " ";
			}
			cout << ", time: " << routes_time[i]; 
			cout << "\n\n";
		}
	}
}
void SavingsAlgo::do_savings_algo(){
	while(1){
		// find a valid savings node
		SavingsNode sn = find_valid_sn();

		// this means we cannot no longer find a valid savings node, so end this algo. 
		if(sn.value == -1)
			break;

		// merge the route of ni, nj in savings node
		int sn_i = sn.i;
		int sn_j = sn.j;

		merge_route(sn_i,sn_j);
	}
}
float SavingsAlgo::get_dist(Node n1, Node n2){
	return sqrt((n1.x-n2.x)*(n1.x-n2.x) + (n1.y-n2.y)*(n1.y-n2.y));
}
bool descend_func(SavingsNode sn1, SavingsNode sn2){
	return sn1.value > sn2.value;
}
bool SavingsAlgo::check_sn(SavingsNode sn){
	// check if two customers are in the same route
	if(routes_map[sn.i] == routes_map[sn.j]){
		//cout << "same route" << endl;
		return false;
	}
	// check if both customer nodes are not the interior node of their route
	vector<int> route_i = routes_table[routes_map[sn.i]];
	vector<int> route_j = routes_table[routes_map[sn.j]];
	if(route_i[0] != sn.i && route_i[route_i.size()-1] != sn.i){
		return false;
	}
	if(route_j[0] != sn.j && route_j[route_j.size()-1] != sn.j){
		return false;
	}
	// check time limit after merge 
	float ri_dist, rj_dist, totoal_dist;
	float max_dist = SPEED*T;
	Node ri_start = customer_points[route_i[0]];
	Node ri_end = customer_points[route_i[route_i.size()-1]];
	Node rj_start = customer_points[route_j[0]];
	Node rj_end = customer_points[route_j[route_j.size()-1]];
	ri_dist = get_dist(ri_start,exch_point);
	rj_dist = get_dist(rj_end,exch_point);
	if(route_i.size() >= 2){
		for(int i = 0;i < route_i.size()-1;i++){
			Node n1 = customer_points[route_i[i]];
			Node n2 = customer_points[route_i[i+1]];
			ri_dist += get_dist(n1,n2);
		}
	} 
	if(route_j.size() >= 2){
		for(int i = 0;i < route_j.size()-1;i++){
			Node n1 = customer_points[route_j[i]];
			Node n2 = customer_points[route_j[i+1]];
			rj_dist += get_dist(n1,n2);
		}
	}
	totoal_dist = ri_dist + rj_dist + get_dist(ri_end,rj_start);
	if(totoal_dist > max_dist){
		//cout << "exceed time limit" << endl;
		return false;
	}
	return true;
}
void SavingsAlgo::delete_maxP_sn(int max_p){
	// delete first max_p saving nodes in savings_list
	savings_list.erase(savings_list.begin(),savings_list.begin()+max_p);
}
void SavingsAlgo::get_savings(){
	// s_ij = d_i0 + d_0j - d_ij
	Node n0 = exch_point;
	for(int i = 0;i < customer_num;i++){
		for(int j = i+1;j < customer_num;j++){
			// prevent calculating savings value of same node
			if(i==j) continue;
			Node ni = customer_points[i];
			Node nj = customer_points[j];
			float tmp = get_dist(n0,ni) + get_dist(n0,nj) - get_dist(ni,nj);
			SavingsNode sn(tmp,i,j,ni,nj);
			savings_list.push_back(sn);
		}
	}
	// sort savings list descending
	sort(savings_list.begin(),savings_list.end(),descend_func);
}
SavingsNode SavingsAlgo::find_valid_sn(){
	// randomly choose one from first P saving nodes
	// which satisfy certain conditions
	int max_p;
	SavingsNode sn;
	bool is_sn_valid = false;

	while(!is_sn_valid){
		// make a random list range from 0 ~ (max_p - 1)
		max_p  = savings_list.size() >= RSA_P ? RSA_P:savings_list.size();
		vector<int> n_list; // a random number list range from 0 ~ (max_p - 1)
		for(int i = 0;i < max_p;i++){
			n_list.push_back(i);
		}
		shuffle(n_list.begin(),n_list.end(),default_random_engine(time(NULL)));
		for(int i = 0;i < max_p;i++){
			sn = savings_list[n_list[i]];
			if(check_sn(sn)){
				// set flag
				is_sn_valid = true;
				// delete valid sn in savings_list
				savings_list.erase(savings_list.begin()+n_list[i]);
				break;
			}
		}
		// if first max_p savings node cannot satisfy conditions, delete them
		if(!is_sn_valid){
			delete_maxP_sn(max_p);
		}
		// if savings_list is empty, return
		if(savings_list.size() == 0){
			SavingsNode tmp;
			sn = tmp;
			break;
		}
	}
	return sn;
}
void SavingsAlgo::merge_route(int sn_i, int sn_j){

	// get route i's index number
	int ri_idx = routes_map[sn_i];
	int rj_idx = routes_map[sn_j];

	// get two target routes
	vector<int> route_i = routes_table[routes_map[sn_i]];
	vector<int> route_j = routes_table[routes_map[sn_j]];

	for(int i = 0;i < route_j.size();i++){
		// regsiter node in route_j into route_i
		routes_map[route_j[i]] = ri_idx;
		// add node in route_j into route_i
		route_i.push_back(route_j[i]);
		//
		routes_flg[route_j[i]] = false;
		
	}

	// use new route_i to replace old route_i
	routes_table[routes_map[sn_i]].clear();
	for(int i = 0;i < route_i.size();i++){
		routes_table[routes_map[sn_i]].push_back(route_i[i]);
	}

}

