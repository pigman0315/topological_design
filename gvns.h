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
//
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
//
class GVNS{
public:
	// variables
	SolutionNode solution;
	vector<Node> customer_points;
	Node exch_point;
	int customer_num;
	const time_t t_max = 60;
	const int k_max = 5;
public:
	// functions
	GVNS(SolutionNode sn, vector<Node> cps, Node ep);
	void initial();
	void run();
	void show_result();
	vector<SolutionNode> build_shake_ns(int k);
	void do_shake(int k);
	void do_VND();
	void comb_go(int offset, int k, vector<int> &combination, vector<int> people,vector< vector<int> > &result);
	bool check_comb(vector<int> vev);
};

GVNS::GVNS(SolutionNode sn, vector<Node> cps, Node ep){
	exch_point = ep;
	customer_points = cps;
	solution = sn;
	customer_num = customer_points.size();
}
void GVNS::run(){
	int k = 2;
	do_shake(k);
}
void GVNS::do_shake(int k){
	// get neighborhood structures of shaking
	vector<SolutionNode> ns = build_shake_ns(k);
	ns[0].show_routes();
	//
}
void GVNS::comb_go(int offset, int k, vector<int> &combination, vector<int> people,vector< vector<int> > &result) {
  if (k == 0) {
    result.push_back(combination);
    return;
  }
  for (int i = offset; i <= people.size() - k; ++i) {
    combination.push_back(people[i]);
    comb_go(i+1, k-1,combination,people,result);
    combination.pop_back();
  }
}
bool GVNS::check_comb(vector<int> vec){
	for(int i = 0;i < vec.size()-1;i++){
		if(vec[i+1]-vec[i]<=1)
			return false;
	}
	return true;
}	
vector<SolutionNode> GVNS::build_shake_ns(int k){
	vector<SolutionNode> sn_vec;
	// do intra-route Or-opt
	if(k == 1){
		const int NUM_OF_NODE = 4;
		vector< vector<int> > rt = solution.routes_table;
		for(int i = 0;i < solution.route_num;i++){
			vector<int> route = rt[i];
			int route_len = route.size();
			if(route_len > NUM_OF_NODE){
				for(int j = 0;j < route_len-NUM_OF_NODE+1;j++){
					for(int l = 0;l < route_len-NUM_OF_NODE+1;l++){
						vector<int> insert_vec;
						insert_vec.assign(route.begin()+j,route.begin()+j+NUM_OF_NODE);
						vector<int> tmp_route = route;
						tmp_route.erase(tmp_route.begin()+j,tmp_route.begin()+j+NUM_OF_NODE);
						tmp_route.insert(tmp_route.begin()+l,insert_vec.begin(),insert_vec.end());
						vector< vector<int> > tmp_rt = rt;
						tmp_rt[i] = tmp_route;
						SolutionNode tmp_sn(tmp_rt);
						sn_vec.push_back(tmp_sn);
					}
				}
			}
		}
	}
	// do intra-route double-bridge
	else if(k == 2){
		const int NUM_OF_NODE = 4;
		vector< vector<int> > rt = solution.routes_table;
		for(int i = 0;i < 1;i++){
			vector<int> route = rt[i];
			int route_len = route.size();
			if(route_len > NUM_OF_NODE){
				// get all combinations of C(n,k)
				vector<int> people;
				vector<int> combination;
				vector< vector<int> > result;
				int n = route_len+1, k = NUM_OF_NODE;
				for (int i = 0; i < n; ++i) { people.push_back(i+1); }
				comb_go(0, k,combination, people, result);
				for(int r = 0;r < result.size();r++){
					// check if the edges are not adjacency
					if(check_comb(result[r])){
						vector<int> A,B,C,D,tmp_v;
						A.assign(route.begin(),route.begin()+result[r][0]+1);
						B.assign(route.begin()+result[r][0]+1,route.begin()+result[r][1]+1);
						C.assign(route.begin()+result[r][1]+1,route.begin()+result[r][2]+1);
						D.assign(route.begin()+result[r][2]+1,route.end());
						tmp_v.insert(tmp_v.end(),A.begin(),A.end());
						tmp_v.insert(tmp_v.end(),D.begin(),D.end());
						tmp_v.insert(tmp_v.end(),C.begin(),C.end());
						tmp_v.insert(tmp_v.end(),B.begin(),B.end());
						vector< vector<int> > tmp_rt = rt;
						tmp_rt[i] = tmp_v;
						SolutionNode tmp_sn(tmp_rt);
						sn_vec.push_back(tmp_sn);
					}
				}
			}
			//
		}
		
		
	}
	return sn_vec;
}