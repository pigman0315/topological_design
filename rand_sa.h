// 
// Implemantation of randomized savings algorithm
//
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
#include <chrono>
#include <cfloat>

#ifndef _NODE_
#define _NODE_
#include "node.h" // class Node/SavingsNode/SolutionNode
#endif

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
extern const float SERV_COST;
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
	vector< vector<float> > dist_table;
	Node exch_point;
	static const int RSA_P = 10; // parameter for randomized savings algo.
	static const int RSA_Q = 10; // parameter for randomized savings algo.
	static const int RSA_U = 2; // parameter in minimize_routes to optimize randomized savings algo.
public:
	// functions
	SavingsAlgo(vector<Node> cps, Node ep, vector< vector<float> >);
	void initial();
	void run();
	void get_savings();
	float get_dist(int n1, int n2);
	bool check_sn(SavingsNode sn);
	void delete_maxP_sn(int max_p);
	SavingsNode find_valid_sn();
	void merge_route(int sn_i, int sn_j);
	void do_savings_algo();
	void show_routes();
	void minimize_routes();
	void get_routes_time();
	bool insert_node(Node n_insert, int n_insert_num);
	float get_total_travel_time();
	SolutionNode get_solution();
};
// constructor
SavingsAlgo::SavingsAlgo(vector<Node> cps, Node ep, vector< vector<float> > dist_table_){
	customer_points = cps;
	exch_point = ep;
	customer_num = customer_points.size();
	dist_table = dist_table_;
	routes_table.clear();
	routes_map.clear();
	routes_flg.clear();
	routes_time.clear();
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
	// do randomized savings algo to find initial routes
	do_savings_algo();
	// try to minimize the number of routes
	minimize_routes();
}
void SavingsAlgo::initial(){
	routes_table.clear();
	routes_map.clear();
	routes_flg.clear();
	routes_time.clear();
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
SolutionNode SavingsAlgo::get_solution(){
	SolutionNode sn(routes_table,routes_flg,routes_time,customer_num);
	return sn;
}
void SavingsAlgo::get_routes_time(){
	for(int i = 0;i < customer_num;i++){
		float sum = 0.0;
		if(routes_flg[i] == true){
			// sum += get_dist(exch_point,customer_points[routes_table[i][0]]);
			sum += get_dist(-1,routes_table[i][0]);
			for(int j = 0;j < routes_table[i].size();j++){
				if(j < routes_table[i].size()-1)
					// sum += get_dist(customer_points[routes_table[i][j]],customer_points[routes_table[i][j+1]]);
					sum += get_dist(routes_table[i][j],routes_table[i][j+1]);
			}
			// sum += get_dist(customer_points[routes_table[i][routes_table[i].size()-1]],exch_point);
			sum += get_dist(routes_table[i][routes_table[i].size()-1],-1);
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
					int n_last = routes_table[i][routes_table[i].size()-1];
					// add_dist = get_dist(exch_point,n_insert)+get_dist(n_insert,n_last)-get_dist(exch_point,n_last);
					add_dist = get_dist(-1,n_insert_num)+get_dist(n_insert_num,n_last)-get_dist(-1,n_last);
				}
				else{
					// Node n_cur = customer_points[routes_table[i][j]]; // current node in routes
					int n_cur = routes_table[i][j];
					if(j == 0 || j == routes_table[i].size()){
						// add_dist = get_dist(exch_point,n_insert)+get_dist(n_insert,n_cur)-get_dist(exch_point,n_cur);
						add_dist = get_dist(-1,n_insert_num)+get_dist(n_insert_num,n_cur)-get_dist(-1,n_cur);
					}
					else{
						// Node n_prev = customer_points[routes_table[i][j-1]];
						int n_prev = routes_table[i][j-1];
						add_dist = get_dist(n_insert_num,n_prev)+get_dist(n_insert_num,n_cur)-get_dist(n_prev,n_cur);
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
	vector<int> single_node_nums; // nodes which is released
	// release routes
	// i = 1, do not release the first route to prevent unknown error
	for(int i = 1;i < routes_table.size();i++){
		if(routes_table[i].size() <= RSA_U && routes_flg[i] == true){
			for(int j = 0;j < routes_table[i].size();j++){
				routes_table[routes_table[i][j]].clear();
				routes_table[routes_table[i][j]].push_back(routes_table[i][j]);
				routes_map[routes_table[i][j]] = routes_table[i][j];
				single_node_nums.push_back(routes_table[i][j]);
			}
			routes_table[i].clear();
			routes_table[i].push_back(i);
			routes_flg[i] = false;
		}
	}

	// cout << "Total release node numbers: " << single_node_nums.size() << endl;
	vector<int> failed_nodes; // nodes which fail to be inserted into existed routes
	// insert those single node into current available routes
	for(int n = 0;n < single_node_nums.size();n++){
		//cout << single_node_nums[n] << endl;
		Node n_insert = customer_points[single_node_nums[n]]; // node we want to insert 
		int node_num = single_node_nums[n];
		if(!insert_node(n_insert, node_num)){
			failed_nodes.push_back(single_node_nums[n]);
			// cout << "insert node " << single_node_nums[n] << " failed." << endl;
		}
	}
	// rebuild routes for those nodes which cannot be inserted into existed routes
	if(failed_nodes.size() >= 1){
		float min_dist = FLT_MAX;
		int prev_node;
		int cur_node;
		int min_node;
		int min_idx;
		int cur_rn; // current route number
		float cur_dist;
		// find a node closest to be first node
		for(int i = 0;i < failed_nodes.size();i++){
			cur_node = failed_nodes[i];
			cur_dist = get_dist(cur_node,-1);
			if(cur_dist < min_dist){
				min_dist = cur_dist;
				min_idx = i;
				cur_rn = failed_nodes[i];
			}
		}

		routes_flg[cur_rn] = true;
		failed_nodes.erase(failed_nodes.begin()+min_idx);
		prev_node = cur_node;

		// do while until number of failed nodes = 0
		while(failed_nodes.size() > 0){
			min_idx = -1;
			min_dist = FLT_MAX;
			for(int i = 0;i < failed_nodes.size();i++){
				cur_node = failed_nodes[i];
				cur_dist = get_dist(cur_node,prev_node);
				if(cur_dist < min_dist){
					min_dist = cur_dist;
					min_idx = i;
					min_node = cur_node;
				}
			}

			float add_dist = cur_dist + get_dist(-1,min_node) - get_dist(-1,prev_node);
			float add_time = add_dist / SPEED;
			
			if((add_time+routes_time[cur_rn]) <= T && min_idx != -1){
				routes_table[cur_rn].push_back(failed_nodes[min_idx]);
				routes_map[failed_nodes[min_idx]] = cur_rn;
				routes_time[cur_rn] += add_time;
				failed_nodes.erase(failed_nodes.begin()+min_idx);
				prev_node = min_node;
			}

			// find anothor start point if exceed time limit
			else{
				min_dist = FLT_MAX;
				for(int i = 0;i < failed_nodes.size();i++){
					cur_node = failed_nodes[i];
					cur_dist = get_dist(cur_node,-1);
					if(cur_dist < min_dist){
						min_dist = cur_dist;
						min_idx = i;
						prev_node = failed_nodes[i];
						cur_rn = prev_node;
					}
				}
				routes_flg[cur_rn] = true;
				failed_nodes.erase(failed_nodes.begin()+min_idx);
			}

		}
	}
	get_routes_time();	
}
void SavingsAlgo::show_routes(){
	int cnt = 0;
	for(int i = 0;i < customer_num;i++){
		if(routes_flg[i] == true){
			for(int j = 0;j < routes_table[i].size();j++){
				cnt++;
				cout << routes_table[i][j] << " ";
			}
			cout << ", time: " << routes_time[i]; 
			cout << endl;
		}
	}
	cout << "Total node number: " << cnt << endl;
}
void SavingsAlgo::do_savings_algo(){
	float min_time = FLT_MAX;
	vector< vector<int> > best_routes_table;// be used to record routes with customer index of customer_points
	map<int,int> best_routes_map; // be used to map customer with his route number
	vector<bool> best_routes_flg; // to show which routes in map is available
	vector<float> best_routes_time; // store each routes' time
	bool update = false;
	// do Q times to find best routes(Randomized Savings algo.)
	for(int i = 0;i < RSA_Q;i++){
		// initial routes
		initial();

		// calculate savings value for each pair of node
		get_savings();
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
		// get each route's travel time
		get_routes_time();

		// compare current travel time with best
		float cur_time = get_total_travel_time();
		if(cur_time < min_time){
			min_time = cur_time;
			best_routes_table = routes_table;
			best_routes_map = routes_map;
			best_routes_time = routes_time;
			best_routes_flg = routes_flg;
			update = true;
		}
	}
	if(update == true){
		routes_table = best_routes_table;
		routes_map = best_routes_map;
		routes_flg = best_routes_flg;
		routes_time = best_routes_time;
	}
}
float SavingsAlgo::get_dist(int n1, int n2){
	if(n1 == -1)
		return dist_table[n2][n2];
	if(n2 == -1)
		return dist_table[n1][n1];
	return dist_table[n1][n2];
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
	int ri_start = route_i[0];
	int ri_end = route_i[route_i.size()-1];
	int rj_start = route_j[0];
	int rj_end = route_j[route_j.size()-1];
	ri_dist = get_dist(ri_start,-1);
	rj_dist = get_dist(rj_end,-1);
	if(route_i.size() >= 2){
		for(int i = 0;i < route_i.size()-1;i++){
			int n1 = route_i[i];
			int n2 = route_i[i+1];
			ri_dist += get_dist(n1,n2);
		}
	} 
	if(route_j.size() >= 2){
		for(int i = 0;i < route_j.size()-1;i++){
			int n1 = route_j[i];
			int n2 = route_j[i+1];
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
			float tmp = get_dist(-1,i) + get_dist(-1,j) - get_dist(i,j);
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
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		shuffle(n_list.begin(),n_list.end(),default_random_engine(seed));
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
float SavingsAlgo::get_total_travel_time(){
	float sum = 0.0;
	for(int i = 0;i < customer_num;i++){
		if(routes_flg[i] == true){
			sum += routes_time[i];
		}
	}
	return sum;
}

