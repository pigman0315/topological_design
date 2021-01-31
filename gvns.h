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
	static const time_t t_max = 60;
	static const int k_max = 5;
	static const int l_max = 8;
public:
	GVNS(SolutionNode sn, vector<Node> cps, Node ep);
	void initial();
	void run();
	void show_result();
	// for shake
	vector<SolutionNode> build_shake_ns(int k);
	SolutionNode do_shake(int k);
	void comb_go(int offset, int k, vector<int> &combination, vector<int> people,vector< vector<int> > &result);
	bool check_comb(vector<int> vec);
	vector<SolutionNode> shake_ns1(); // shake phase's neighborhood structure 1: intra-route or-opt
	vector<SolutionNode> shake_ns2(); // shake phase's neighborhood structure 2: intra-route double-bridge
	vector<SolutionNode> shake_ns3(); // shake phase's neighborhood structure 3: inter-route or-opt
	vector<SolutionNode> shake_ns4(); // shake phase's neighborhood structure 4: inter-route cross-exchange
	vector<SolutionNode> shake_ns5(); // shake phase's neighborhood structure 5: inter-route icross-exchange
	// for VND
	SolutionNode do_VND();
	vector<SolutionNode> build_VND_ns(SolutionNode cur_sn, int l);
	vector<SolutionNode> VND_ns1(SolutionNode cur_sn); // VND phase's neighborhood structure 1: intra-route 2-opt
	vector<SolutionNode> VND_ns2(SolutionNode cur_sn); // VND phase's neighborhood structure 2: intra-route Or-opt
	vector<SolutionNode> VND_ns3(SolutionNode cur_sn); // VND phase's neighborhood structure 3: intra-route 3-opt
	vector<SolutionNode> VND_ns4(SolutionNode cur_sn); // VND phase's neighborhood structure 4: inter-route shift(1,0)
	vector<SolutionNode> VND_ns5(SolutionNode cur_sn); // VND phase's neighborhood structure 5: inter-route swap(1,1)
	vector<SolutionNode> VND_ns6(SolutionNode cur_sn); // VND phase's neighborhood structure 6: inter-route or-opt
	vector<SolutionNode> VND_ns7(SolutionNode cur_sn); // VND phase's neighborhood structure 7: inter-route cross-exchange
	vector<SolutionNode> VND_ns8(SolutionNode cur_sn); // VND phase's neighborhood structure 8: inter-route icross-exchange
};

GVNS::GVNS(SolutionNode sn, vector<Node> cps, Node ep){
	exch_point = ep;
	customer_points = cps;
	solution = sn;
	customer_num = customer_points.size();
}
void GVNS::run(){
	int k = 1;
	SolutionNode sn1, sn2;
	sn1 = do_shake(k);
	sn2 = do_VND();
}
SolutionNode GVNS::do_VND(){
	SolutionNode cur_sn = solution;
	int l = 2;

	// get neighborhood structures l of current solution node in VND
	vector<SolutionNode> ns = build_VND_ns(cur_sn,l);
	cout << ns.size() << endl;
	ns[2].show();
	// result result
	return cur_sn;
}

SolutionNode GVNS::do_shake(int k){
	// get neighborhood structures of shaking
	vector<SolutionNode> ns = build_shake_ns(k);
	
	// randomly choose one solution from neighborhood structure of k
	int n = ns.size();
	int rand_num = rand()%n;

	// return result
	return ns[rand_num];
}

vector<SolutionNode> GVNS::build_VND_ns(SolutionNode cur_sn, int l){
	vector<SolutionNode> sn_vec;
	switch(l){
		case 1:
			sn_vec = VND_ns1(cur_sn);
			break;
		case 2:
			sn_vec = VND_ns2(cur_sn);
			break;
		case 3:
			sn_vec = VND_ns3(cur_sn);
			break;
		case 4:
			sn_vec = VND_ns4(cur_sn);
			break;
		case 5:
			sn_vec = VND_ns5(cur_sn);
			break;
		case 6:
			sn_vec = VND_ns6(cur_sn);
			break;
		case 7:
			sn_vec = VND_ns7(cur_sn);
			break;
		case 8:
			sn_vec = VND_ns8(cur_sn);
			break;
	}
	return sn_vec;
}

vector<SolutionNode> GVNS::VND_ns1(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	const int NUM_OF_NODE = 2;
	//
	// TODO: 2-opt
	// 
	for(int i = 0; i < rt.size();i++){
		// get all combinations of C(n,k)
		if(rt[i].size() < 3)
			continue;
		vector<int> people;
		vector<int> combination;
		vector< vector<int> > result;
		vector<int> route = rt[i];
		int route_len = rt[i].size();
		int n = route_len+1, k = NUM_OF_NODE;
		for (int j = 0; j < n; j++) { people.push_back(j+1); }
		comb_go(0, k,combination, people, result);
		for(int j = 0;j < result.size();j++){
			if(check_comb(result[j])){
				vector<int> seg;
				seg.assign(route.begin()+result[j][0]-1,route.begin()+result[j][1]-1);
				vector<int> tmp_route = route;
				tmp_route.erase(tmp_route.begin()+result[j][0]-1,tmp_route.begin()+result[j][1]-1);
				for(int k = 0;k < seg.size();k++){
					tmp_route.insert(tmp_route.begin()+result[j][0]-1+k,seg[seg.size()-1-k]);
				}
				vector< vector<int> > tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn);
			}
		}
	}
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns2(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	//
	// TODO: intra-route Or-opt
	//
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	for(int i = 0;i < route_num;i++){
		int route_len = rt[i].size();
		vector<int> route = rt[i];
		// determine segment length of or-opt
		int seg_len;
		if(rt[i].size() == 1) continue;
		else if(rt[i].size() == 2){
			seg_len = rand() % 2 + 1;
		}
		else{
			seg_len = rand() % 3 + 1;
		}
		//
		for(int j = 0;j < route_len - seg_len + 1;j++){
			vector<int> seg;
			seg.assign(route.begin()+j,route.begin()+j+seg_len);
			for(int k = 0;k < route_len - seg_len + 1;k++){
				vector<int> tmp_route = route;
				tmp_route.erase(tmp_route.begin()+j,tmp_route.begin()+j+seg_len);
				tmp_route.insert(tmp_route.begin()+k,seg.begin(),seg.end());
				vector< vector<int> > tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn);
			}
		}
	}

	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns3(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	//
	// TODO: intra-route 3-opt
	//
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns4(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	//
	// TODO: inter-route shift(1,0)
	//
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns5(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	//
	// TODO: inter-route swap(1,1)
	//
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns6(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	//
	// TODO: inter-route Or-opt
	//
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns7(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	//
	// TODO: inter-route cross-exchange
	//
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns8(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	//
	// TODO: inter-route icross-exchange
	//
	return sn_vec;
}

vector<SolutionNode> GVNS::build_shake_ns(int k){
	vector<SolutionNode> sn_vec;
	switch(k){
		case 1:
			// do intra-route Or-opt
			sn_vec = shake_ns1();
			break;
		case 2:
			// do intra-route double-bridge
			sn_vec = shake_ns2();
			break;
		case 3:
			// do inter-route Or-opt
			sn_vec = shake_ns3();
			break;
		case 4:
			// do inter-route cross-exchange
			sn_vec = shake_ns4();
			break;
		case 5:
			// do inter-route icross-exchange
			sn_vec = shake_ns5();
			break;
	}
	return sn_vec;
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

vector<SolutionNode> GVNS::shake_ns1(){
	vector<SolutionNode> sn_vec;
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
					SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
					sn_vec.push_back(tmp_sn);
				}
			}
		}
	}
	return sn_vec;
}

vector<SolutionNode> GVNS::shake_ns2(){
	vector<SolutionNode> sn_vec;
	const int NUM_OF_NODE = 4;
	vector< vector<int> > rt = solution.routes_table;
	for(int i = 0;i < solution.route_num;i++){
		vector<int> route = rt[i];
		int route_len = route.size();
		if(route_len > NUM_OF_NODE){
			// get all combinations of C(n,k)
			vector<int> people;
			vector<int> combination;
			vector< vector<int> > result;
			int n = route_len+1, k = NUM_OF_NODE;
			for (int j = 0; j < n; j++) { people.push_back(j+1); }
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
					SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
					sn_vec.push_back(tmp_sn);
				}
			}
		}
	}
	return sn_vec;
}

vector<SolutionNode> GVNS::shake_ns3(){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = solution.routes_table;
	for(int i = 0;i < solution.route_num;i++){
		vector<int> route = rt[i];
		// determine the length of segments: [4,min(6,n)]
		int n = rt[i].size();
		const int MIN_LEN = 4;
		const int MAX_LEN = 6;
		int NUM_OF_NODE;
		if(n > MIN_LEN){
			if(n >= MAX_LEN){
				NUM_OF_NODE = rand() % 3 + MIN_LEN;
			}
			else{
				NUM_OF_NODE = rand() % 2 + MIN_LEN;
			}
			for(int j = 0;j < n-NUM_OF_NODE+1;j++){
				vector<int> insert_vec, delete_vec;
				insert_vec.assign(route.begin()+j,route.begin()+j+NUM_OF_NODE);
				delete_vec = route;
				delete_vec.erase(delete_vec.begin()+j,delete_vec.begin()+j+NUM_OF_NODE);
				for(int k = 0;k < solution.route_num;k++){
					if(k == i)
						continue;
					for(int l = 0;l < rt[k].size();l++){
						vector<int> tmp_vec;
						tmp_vec = rt[k];
						tmp_vec.insert(tmp_vec.begin()+l,insert_vec.begin(),insert_vec.end());
						vector< vector<int> > tmp_rt = rt;
						tmp_rt[k] = tmp_vec;
						tmp_rt[i] = delete_vec;
						if(delete_vec.size() == 0){
							tmp_rt.erase(tmp_rt.begin()+i);
						}
						SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
						sn_vec.push_back(tmp_sn);
					}
				}
			}
		}
	}
	return sn_vec;
}

vector<SolutionNode> GVNS::shake_ns4(){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = solution.routes_table;
	for(int i = 0;i < solution.route_num;i++){
		vector<int> route1 = rt[i];
		// determine the length of segments: [4,min(6,n)]
		const int MIN_LEN = 4;
		const int MAX_LEN = 6;
		int NUM_OF_NODE1;
		if(rt[i].size() > MIN_LEN){
			if(rt[i].size() >= MAX_LEN){
				NUM_OF_NODE1 = rand() % 3 + MIN_LEN;
			}
			else{
				NUM_OF_NODE1 = rand() % 2 + MIN_LEN;
			}
			for(int j = 0;j < rt[i].size()-NUM_OF_NODE1+1;j++){
				vector<int> seg1;
				seg1.assign(route1.begin()+j,route1.begin()+j+NUM_OF_NODE1);
				for(int k = 0;k < solution.route_num;k++){
					if(k == i)
						continue;
					int NUM_OF_NODE2;
					if(rt[k].size() > MIN_LEN){
						if(rt[k].size() >= MAX_LEN){
							NUM_OF_NODE2 = rand() % 3 + MIN_LEN;
						}
						else{
							NUM_OF_NODE2 = rand() % 2 + MIN_LEN;
						}
						vector<int> route2 = rt[k];
						for(int l = 0;l < rt[k].size()-NUM_OF_NODE2+1;l++){
							vector<int> seg2;
							seg2.assign(route2.begin()+l,route2.begin()+l+NUM_OF_NODE2);
							//
							vector<int> tmp_r1 = route1;
							vector<int> tmp_r2 = route2;
							tmp_r1.erase(tmp_r1.begin()+j,tmp_r1.begin()+j+NUM_OF_NODE1);
							tmp_r1.insert(tmp_r1.begin()+j,seg2.begin(),seg2.end());
							tmp_r2.erase(tmp_r2.begin()+l,tmp_r2.begin()+l+NUM_OF_NODE2);
							tmp_r2.insert(tmp_r2.begin()+l,seg1.begin(),seg1.end());
							//
							vector< vector<int> > tmp_rt = rt;
							tmp_rt[i] = tmp_r1;
							tmp_rt[k] = tmp_r2;
							SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
							sn_vec.push_back(tmp_sn);
						}
					}
					else{
						continue;
					}
				}
			}
		}
	}
	return sn_vec;
}

vector<SolutionNode> GVNS::shake_ns5(){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = solution.routes_table;
	for(int i = 0;i < solution.route_num;i++){
		vector<int> route1 = rt[i];
		// determine the length of segments: [4,min(6,n)]
		const int MIN_LEN = 4;
		const int MAX_LEN = 6;
		int NUM_OF_NODE1;
		if(rt[i].size() > MIN_LEN){
			if(rt[i].size() >= MAX_LEN){
				NUM_OF_NODE1 = rand() % 3 + MIN_LEN;
			}
			else{
				NUM_OF_NODE1 = rand() % 2 + MIN_LEN;
			}
			for(int j = 0;j < rt[i].size()-NUM_OF_NODE1+1;j++){
				vector<int> seg1;
				seg1.assign(route1.begin()+j,route1.begin()+j+NUM_OF_NODE1);
				for(int k = 0;k < solution.route_num;k++){
					if(k == i)
						continue;
					int NUM_OF_NODE2;
					if(rt[k].size() > MIN_LEN){
						if(rt[k].size() >= MAX_LEN){
							NUM_OF_NODE2 = rand() % 3 + MIN_LEN;
						}
						else{
							NUM_OF_NODE2 = rand() % 2 + MIN_LEN;
						}
						vector<int> route2 = rt[k];
						for(int l = 0;l < rt[k].size()-NUM_OF_NODE2+1;l++){
							vector<int> seg2;
							seg2.assign(route2.begin()+l,route2.begin()+l+NUM_OF_NODE2);
							//
							vector<int> tmp_r1 = route1;
							vector<int> tmp_r2 = route2;
							tmp_r1.erase(tmp_r1.begin()+j,tmp_r1.begin()+j+NUM_OF_NODE1);
							tmp_r2.erase(tmp_r2.begin()+l,tmp_r2.begin()+l+NUM_OF_NODE2);
							for(int a = 0;a < seg2.size();a++){
								tmp_r1.insert(tmp_r1.begin()+j+a,seg2[seg2.size()-1-a]);
							}
							for(int a = 0;a < seg1.size();a++){
								tmp_r2.insert(tmp_r2.begin()+l+a,seg1[seg1.size()-1-a]);
							}
							//
							vector< vector<int> > tmp_rt = rt;
							tmp_rt[i] = tmp_r1;
							tmp_rt[k] = tmp_r2;
							SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
							sn_vec.push_back(tmp_sn);
						}
					}
					else{
						continue;
					}
				}
			}
		}
	}
	return sn_vec;
}