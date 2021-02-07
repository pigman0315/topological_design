#include <iostream>
#include <climits>
#include <string>
#include <map>
#include <cfloat>
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
	int owned_courier_num;
	static const time_t t_max = 30;
	static const int k_max = 5;
	static const int l_max = 8;
	static const int m_max = 5;
	float delta_1;
public:
	GVNS();
	GVNS(SolutionNode sn, vector<Node> cps, Node ep);
	GVNS(SolutionNode sn, vector<Node> cps, Node ep, int _owned_courier_num);
	void initial();
	void run();
	void show_result();
	// for GVNS
	bool is_better_sol(SolutionNode best_sn, SolutionNode sn);
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
	SolutionNode do_VND(SolutionNode sn1);
	SolutionNode find_best_neighbor(vector<SolutionNode> ns);
	vector<SolutionNode> build_VND_ns(SolutionNode cur_sn, int l);
	vector<SolutionNode> VND_ns1(SolutionNode cur_sn); // VND phase's neighborhood structure 1: intra-route 2-opt
	vector<SolutionNode> VND_ns2(SolutionNode cur_sn); // VND phase's neighborhood structure 2: intra-route Or-opt
	vector<SolutionNode> VND_ns3(SolutionNode cur_sn); // VND phase's neighborhood structure 3: intra-route 3-opt
	vector<SolutionNode> VND_ns4(SolutionNode cur_sn); // VND phase's neighborhood structure 4: inter-route shift(1,0)
	vector<SolutionNode> VND_ns5(SolutionNode cur_sn); // VND phase's neighborhood structure 5: inter-route swap(1,1)
	vector<SolutionNode> VND_ns6(SolutionNode cur_sn); // VND phase's neighborhood structure 6: inter-route or-opt
	vector<SolutionNode> VND_ns7(SolutionNode cur_sn); // VND phase's neighborhood structure 7: inter-route cross-exchange
	vector<SolutionNode> VND_ns8(SolutionNode cur_sn); // VND phase's neighborhood structure 8: inter-route icross-exchange
	// for workload balance
	void do_work_balance();
	vector<SolutionNode> VNDI_ns1(SolutionNode cur_sn); // VND-I phase'sneighborhood structure 1: inter-route shift(1,0)
	vector<SolutionNode> VNDI_ns2(SolutionNode cur_sn); // VND-I phase'sneighborhood structure 2: inter-route shift(2,0)
	vector<SolutionNode> build_VNDI_ns(SolutionNode cur_sn, int m);
	SolutionNode find_balance_neighbor(vector<SolutionNode> ns);
};
GVNS::GVNS(SolutionNode sn, vector<Node> cps, Node ep){
	exch_point = ep;
	customer_points = cps;
	solution = sn;
	customer_num = customer_points.size();
	delta_1 = 0.05;
	owned_courier_num = -1;
}
GVNS::GVNS(SolutionNode sn, vector<Node> cps, Node ep,int _owned_courier_num){
	exch_point = ep;
	customer_points = cps;
	solution = sn;
	customer_num = customer_points.size();
	delta_1 = 0.05;
	owned_courier_num = _owned_courier_num;
}
void GVNS::run(){
	time_t t, start_t, end_t;
	t = 0;
	start_t = time(NULL);
	// step 2, 3: improve initial routes & get fixed number of owned routing courier
	while(t < t_max){
		for(int k = 1;k <= k_max;k++){
			SolutionNode sn1, sn2;
			sn1 = do_shake(k);
			sn2 = do_VND(sn1);
			if(is_better_sol(solution,sn2)){
				solution = sn2;
				k = 1;
			}
		}
		end_t = time(NULL);
		t = end_t - start_t;
	}
	// step 4,5: workload balance
	// do_work_balance();
	// solution.show();
	// cout << "Total time(after step4,5) = " << solution.total_time << endl;
}
void GVNS::do_work_balance(){
	// get neighborhood structures m of current solution node in VND-I
	SolutionNode cur_sn = solution;
	int m = 1;
	time_t t = 0;
	time_t start,end;
	start = time(NULL);
	int best_time = solution.total_time;
	float diff;
	float max = 0.0;
	float min = FLT_MAX;
	for(int i = 0;i < cur_sn.route_num;i++){
		if(cur_sn.routes_time[i] > max){
			max = cur_sn.routes_time[i];
		}
		if(cur_sn.routes_time[i] < min){
			min = cur_sn.routes_time[i];
		}
	}
	diff = max - min;
	while(m <= m_max && t < t_max){
		// cout << "m: " << m << endl;
		vector<SolutionNode> ns = build_VNDI_ns(cur_sn,m); 
		// best balance neighbor
		SolutionNode balance_neighbor = find_balance_neighbor(ns);
		max = 0.0;
		min = FLT_MAX;
		float cur_diff;
		int route_len = balance_neighbor.routes_table.size();
		for(int i = 0;i < route_len;i++){
			if(balance_neighbor.routes_time[i] > max){
				max = balance_neighbor.routes_time[i];
			}
			if(balance_neighbor.routes_time[i] < min){
				min = balance_neighbor.routes_time[i];
			}
		}
		cur_diff = max - min;
		//cout << "cur diff: " << cur_diff << ", best diff: " << diff << endl;
		if(solution.total_time*(1+delta_1) > balance_neighbor.total_time && cur_diff < diff){
			diff = cur_diff;
			cur_sn = balance_neighbor;
			m = 1;
		}
		else{
			m = m + 1;
		}
		end = time(NULL);
		t = end - start;
	}
	solution = cur_sn;
}
SolutionNode GVNS::find_balance_neighbor(vector<SolutionNode> ns){
	vector<SolutionNode> illegal_ns;
	for(int i = 0;i < ns.size();i++){
		int cnt = 0;
		SolutionNode cur_sn = ns[i];
		for(int j = 0;j < cur_sn.routes_time.size();j++){
			if(cur_sn.routes_time[j] < T)
				cnt++;
		}
		if(cnt == cur_sn.routes_time.size()){
			illegal_ns.push_back(cur_sn);
		}
	}
	float diff = FLT_MAX;
	SolutionNode best_sn;
	for(int i = 0;i < illegal_ns.size();i++){
		SolutionNode cur_sn = illegal_ns[i];
		float min = FLT_MAX;
		float max = 0.0;
		for(int j = 0;j < cur_sn.routes_time.size();j++){
			if(cur_sn.routes_time[j] > max)
				max = cur_sn.routes_time[j];
			if(cur_sn.routes_time[j] < min)
				min = cur_sn.routes_time[j];
		}
		float cur_diff = max - min;
		if(cur_diff < diff){
			diff = cur_diff;
			best_sn = cur_sn;
		}
	}
	return best_sn;
}
vector<SolutionNode> GVNS::VNDI_ns1(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	
	// find big & small route
	vector<int> big_routes;
	vector<int> small_routes;
	float avg = 0.0;
	for(int i = 0;i < route_num;i++){
		avg += cur_sn.routes_time[i];
	}
	avg /= rt.size();
	for(int i = 0;i < route_num;i++){
		if(cur_sn.routes_time[i] >= avg){
			big_routes.push_back(i);
		}
		else{
			small_routes.push_back(i);
		}
	}
 	//
	// TODO: inter-route shift(1,0)
	//
	for(int i = 0;i < big_routes.size();i++){
		vector<int> route = rt[big_routes[i]];
		int route_len = route.size();
		for(int j = 0;j < route_len;j++){
			vector<int> route_i = route;
			int n = route[j];
			route_i.erase(route_i.begin()+j);
			for(int k = 0;k < small_routes.size();k++){
				if(big_routes[i] == small_routes[k]) continue;
				for(int l = 0;l <= rt[small_routes[k]].size();l++){
					vector<int> route_k = rt[small_routes[k]];
					route_k.insert(route_k.begin()+l,n);
					vector< vector<int> > tmp_rt = rt;
					tmp_rt[small_routes[k]] = route_k;
					if(route_i.size() > 0){
						tmp_rt[big_routes[i]] = route_i;
					}
					else{
						tmp_rt.erase(tmp_rt.begin()+big_routes[i]);
					}
					SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
					sn_vec.push_back(tmp_sn);
				}
			}
		}
	}
	return sn_vec;
}
vector<SolutionNode> GVNS::VNDI_ns2(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	
	// find big & small route
	vector<int> big_routes;
	vector<int> small_routes;
	float avg = 0.0;
	for(int i = 0;i < route_num;i++){
		avg += cur_sn.routes_time[i];
	}
	avg /= rt.size();
	for(int i = 0;i < route_num;i++){
		if(cur_sn.routes_time[i] >= avg){
			big_routes.push_back(i);
		}
		else{
			small_routes.push_back(i);
		}
	}
	// cout << "big2: " << big_routes.size() << endl;
 	//
	// TODO: inter-route shift(2,0)
	//
	for(int i = 0;i < big_routes.size();i++){
		vector<int> route = rt[big_routes[i]];
		int route_len = route.size();
		if(route_len < 2)
			continue;
		for(int j = 0;j < route_len-1;j++){
			vector<int> route_i = route;
			int n1 = route[j];
			int n2 = route[j+1];
			route_i.erase(route_i.begin()+j,route_i.begin()+j+2);
			for(int k = 0;k < small_routes.size();k++){
				if(big_routes[i] == small_routes[k]) continue;
				for(int l = 0;l <= rt[small_routes[k]].size();l++){
					vector<int> route_k = rt[small_routes[k]];
					route_k.insert(route_k.begin()+l,n1);
					route_k.insert(route_k.begin()+l+1,n2);
					vector< vector<int> > tmp_rt = rt;
					tmp_rt[small_routes[k]] = route_k;
					if(route_i.size() > 0){
						tmp_rt[big_routes[i]] = route_i;
					}
					else{
						tmp_rt.erase(tmp_rt.begin()+big_routes[i]);
					}
					SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
					sn_vec.push_back(tmp_sn);
				}
			}
		}
	}
	return sn_vec;
}
vector<SolutionNode> GVNS::build_VNDI_ns(SolutionNode cur_sn,int m){
	vector<SolutionNode> sn_vec;
	switch(m){
		case 1:
			// do inter-route shift(1,0)
			sn_vec = VNDI_ns1(cur_sn);
			break;
		case 2:
			// do inter-route shift(2,0)
			sn_vec = VNDI_ns2(cur_sn);
			break;
		case 3:
			// do intra-route 2-opt
			// use the same function as VND
			sn_vec = VND_ns1(cur_sn);
			break;
		case 4:
			// do intra-route or-opt
			// use the same function as VND
			sn_vec = VND_ns2(cur_sn);
			break;
		case 5:
			// do intra-route 3-opt
			// use the same function as VND
			sn_vec = VND_ns3(cur_sn);
			break;
	}
	return sn_vec;
}
SolutionNode GVNS::do_shake(int k){
	// get neighborhood structures of shaking
	vector<SolutionNode> ns = build_shake_ns(k);
	
	// find illegal ns (routes number > owned courier route number)
	vector<SolutionNode> illegal_ns;
	if(owned_courier_num == -1){
		illegal_ns = ns;
	}
	else{
		for(int i = 0;i < ns.size();i++){
			if(ns[i].routes_table.size() == owned_courier_num){
				illegal_ns.push_back(ns[i]);
			}
		}
	}

	// randomly choose one solution from neighborhood structure of k
	int n = illegal_ns.size();
	int rand_num = rand()%n;

	// return result
	return illegal_ns[rand_num];
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
SolutionNode GVNS::do_VND(SolutionNode sn1){
	SolutionNode cur_sn = sn1;
	int l = 1;
	while(l <= l_max){
		// get neighborhood structures l of current solution node in VND
		vector<SolutionNode> ns = build_VND_ns(cur_sn,l);
		// best best neighbor
		SolutionNode best_neighbor = find_best_neighbor(ns);
		if(cur_sn.total_time > best_neighbor.total_time){
			cur_sn = best_neighbor;
			l = 1;
		}
		else{
			l = l + 1;
		}
	}
	
	// result result
	return cur_sn;
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
bool GVNS::is_better_sol(SolutionNode best_sn, SolutionNode sn){
	if(best_sn.total_time <= sn.total_time)
		return false;
	vector<float> r_time = sn.routes_time;
	for(int i = 0;i < r_time.size();i++){
		if(r_time[i] > T)
			return false;
	}
	return true;
}
SolutionNode GVNS::find_best_neighbor(vector<SolutionNode> ns){
	// find illegal ns (routes number > owned courier route number)
	vector<SolutionNode> illegal_ns;
	if(owned_courier_num == -1){
		illegal_ns = ns;
	}
	else{
		for(int i = 0;i < ns.size();i++){
			if(ns[i].routes_table.size() == owned_courier_num){
				illegal_ns.push_back(ns[i]);
			}
		}
	}
	//
	SolutionNode best_sn = illegal_ns[0];
	for(int i = 1;i < illegal_ns.size();i++){
		SolutionNode cur_sn = illegal_ns[i];
		int cnt = 0;
		for(int j = 0;j < cur_sn.routes_time.size();j++){
			if(cur_sn.routes_time[j] < T)
				cnt++;
		}
		if(cnt == cur_sn.routes_time.size() && cur_sn.total_time < best_sn.total_time){
			best_sn = cur_sn;
		}
	}
	return best_sn;
}
vector<SolutionNode> GVNS::VND_ns1(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	const int NUM_OF_NODE = 2;
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	// TODO: intra-route 2-opt
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
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	// TODO: intra-route Or-opt
	//
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
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	const int NUM_OF_NODE = 3;
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	// TODO: intra-route 3-opt
	//
	for(int i = 0;i < route_num;i++){
		// check length > 3
		if(rt[i].size() < 3) continue;
		// get combinations of C(n,3)
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
				//
				// do 7 kinds of connection in 3-opt
				//
				vector<int> seg;
				vector<int> tmp_route;
				vector< vector<int> > tmp_rt;	
				// 1. single 2-opt(0,1)			
				seg.assign(route.begin()+result[j][0]-1,route.begin()+result[j][1]-1);
				tmp_route = route;
				tmp_route.erase(tmp_route.begin()+result[j][0]-1,tmp_route.begin()+result[j][1]-1);
				for(int k = 0;k < seg.size();k++){
					tmp_route.insert(tmp_route.begin()+result[j][0]-1+k,seg[seg.size()-1-k]);
				}
				tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn1(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn1);

				// 2. single 2-opt(0,2)
				seg.assign(route.begin()+result[j][0]-1,route.begin()+result[j][2]-1);
				tmp_route = route;
				tmp_route.erase(tmp_route.begin()+result[j][0]-1,tmp_route.begin()+result[j][2]-1);
				for(int k = 0;k < seg.size();k++){
					tmp_route.insert(tmp_route.begin()+result[j][0]-1+k,seg[seg.size()-1-k]);
				}
				tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn2(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn2);

				// 3. single 2-opt(1,2)
				seg.assign(route.begin()+result[j][1]-1,route.begin()+result[j][2]-1);
				tmp_route = route;
				tmp_route.erase(tmp_route.begin()+result[j][1]-1,tmp_route.begin()+result[j][2]-1);
				for(int k = 0;k < seg.size();k++){
					tmp_route.insert(tmp_route.begin()+result[j][1]-1+k,seg[seg.size()-1-k]);
				}
				tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn3(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn3);

				// split route into A,B,C segments
				vector<int> A,B,C;
				A.assign(route.begin()+result[j][0]-1,route.begin()+result[j][1]-1);
				B.assign(route.begin()+result[j][1]-1,route.begin()+result[j][2]-1);
				C.assign(route.begin()+result[j][2]-1,route.end());
				if(result[j][0] != 1){
					C.insert(C.end(),route.begin(),route.begin()+result[j][0]-1);
				}
				// 4. 3-opt(A,B',C')
				tmp_route.clear();
				tmp_route.insert(tmp_route.end(),A.begin(),A.end());
				for(int k = B.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),B[k]);
				}
				for(int k = C.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),C[k]);
				}
				tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn4(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn4);
				// 5. 3-opt(A',B',C)
				tmp_route.clear();
				for(int k = A.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),A[k]);
				}
				for(int k = B.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),B[k]);
				}
				tmp_route.insert(tmp_route.end(),C.begin(),C.end());
				tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn5(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn5);
				// 6. 3-opt(A',B,C')
				tmp_route.clear();
				for(int k = A.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),A[k]);
				}
				tmp_route.insert(tmp_route.end(),B.begin(),B.end());
				for(int k = C.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),C[k]);
				}
				tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn6(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn6);
				// 7. 3-opt(A',B',C')
				tmp_route.clear();
				for(int k = A.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),A[k]);
				}
				for(int k = B.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),B[k]);
				}
				for(int k = C.size()-1;k>=0;k--){
					tmp_route.insert(tmp_route.end(),C[k]);
				}
				tmp_rt = rt;
				tmp_rt[i] = tmp_route;
				SolutionNode tmp_sn7(tmp_rt,customer_points,exch_point);
				sn_vec.push_back(tmp_sn7);
			}
		}
	}
	
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns4(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	// TODO: inter-route shift(1,0)
	//
	for(int i = 0;i < route_num;i++){
		vector<int> route = rt[i];
		int route_len = route.size();
		for(int j = 0;j < route_len;j++){
			vector<int> route_i = route;
			int n = route[j];
			route_i.erase(route_i.begin()+j);
			for(int k = 0;k < route_num;k++){
				if(i == k) continue;
				for(int l = 0;l <= rt[k].size();l++){
					vector<int> route_k = rt[k];
					route_k.insert(route_k.begin()+l,n);
					vector< vector<int> > tmp_rt = rt;
					tmp_rt[k] = route_k;
					if(route_i.size() > 0){
						tmp_rt[i] = route_i;
					}
					else{
						tmp_rt.erase(tmp_rt.begin()+i);
					}
					SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
					sn_vec.push_back(tmp_sn);
				}
			}
		}
	}
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns5(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	// TODO: inter-route swap(1,1)
	//
	for(int i = 0;i < route_num;i++){
		for(int j = 0;j < rt[i].size();j++){
			for(int k = 0;k < route_num;k++){
				if(i == k) continue;
				for(int l = 0;l < rt[k].size();l++){
					vector<int> route_k = rt[k];
					vector<int> route_i = rt[i];
					int n_j = rt[i][j];
					int n_l = rt[k][l];
					//
					route_i.erase(route_i.begin()+j);
					route_i.insert(route_i.begin()+j,n_l);
					route_k.erase(route_k.begin()+l);
					route_k.insert(route_k.begin()+l,n_j);
					//
					vector< vector<int> > tmp_rt = rt;
					tmp_rt[i] = route_i;
					tmp_rt[k] = route_k;
					SolutionNode tmp_sn(tmp_rt,customer_points,exch_point);
					sn_vec.push_back(tmp_sn);
				}
			}
		}
	}
	return sn_vec;
}
vector<SolutionNode> GVNS::VND_ns6(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	// TODO: inter-route Or-opt
	//
	for(int i = 0;i < route_num;i++){
		vector<int> route = rt[i];
		// determine the length of segments: [2,min(3,n)]
		int n = rt[i].size();
		const int MIN_LEN = 2;
		const int MAX_LEN = 3;
		int NUM_OF_NODE;
		if(n > MIN_LEN){
			NUM_OF_NODE = rand() % 2 + MIN_LEN;
			for(int j = 0;j < n-NUM_OF_NODE+1;j++){
				vector<int> insert_vec, delete_vec;
				insert_vec.assign(route.begin()+j,route.begin()+j+NUM_OF_NODE);
				delete_vec = route;
				delete_vec.erase(delete_vec.begin()+j,delete_vec.begin()+j+NUM_OF_NODE);
				for(int k = 0;k < route_num;k++){
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
vector<SolutionNode> GVNS::VND_ns7(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	// TODO: inter-route cross-exchange
	//
	for(int i = 0;i < route_num;i++){
		vector<int> route1 = rt[i];
		// determine the length of segments: [2,min(3,n)]
		const int MIN_LEN = 2;
		const int MAX_LEN = 3;
		int NUM_OF_NODE1;
		if(rt[i].size() > MIN_LEN){
			NUM_OF_NODE1 = rand() % 2 + MIN_LEN;
			for(int j = 0;j < rt[i].size()-NUM_OF_NODE1+1;j++){
				vector<int> seg1;
				seg1.assign(route1.begin()+j,route1.begin()+j+NUM_OF_NODE1);
				for(int k = 0;k < route_num;k++){
					if(k == i)
						continue;
					int NUM_OF_NODE2;
					if(rt[k].size() > MIN_LEN){
						NUM_OF_NODE2 = rand() % 2 + MIN_LEN;
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
vector<SolutionNode> GVNS::VND_ns8(SolutionNode cur_sn){
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = cur_sn.routes_table;
	int route_num = rt.size();
	//
	SolutionNode tmp_sn(rt,customer_points,exch_point);
	sn_vec.push_back(tmp_sn);
	//
	//
	// TODO: inter-route icross-exchange
	//
	for(int i = 0;i < route_num;i++){
		// determine the length of segments: [2,min(3,n)]
		const int MIN_LEN = 2;
		const int MAX_LEN = 3;
		int NUM_OF_NODE1;
		if(rt[i].size() >= MIN_LEN){
			vector<int> route1 = rt[i];
			if(rt[i].size() == MIN_LEN){NUM_OF_NODE1 = 2;}
			else{ NUM_OF_NODE1 = rand() % 2 + MIN_LEN;}
			for(int j = 0;j < rt[i].size()-NUM_OF_NODE1+1;j++){
				vector<int> seg1;
				seg1.assign(route1.begin()+j,route1.begin()+j+NUM_OF_NODE1);
				for(int k = 0;k < route_num;k++){
					if(k == i)
						continue;
					int NUM_OF_NODE2;
					if(rt[k].size() >= MIN_LEN){
						if(rt[k].size() == MIN_LEN){ NUM_OF_NODE2 = MIN_LEN; }
						else {NUM_OF_NODE2 = rand() % 2 + MIN_LEN;}
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
				}
			}
		}
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
	//
	// intra-route Or-opt (# of nodes = 4)
	//
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
	//
	// intra-route Double-bridge (# of nodes = 4)
	//
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
					A.assign(route.begin()+result[r][0]-1,route.begin()+result[r][1]-1);
					B.assign(route.begin()+result[r][1]-1,route.begin()+result[r][2]-1);
					C.assign(route.begin()+result[r][2]-1,route.begin()+result[r][3]-1);
					D.assign(route.begin()+result[r][3]-1,route.end());
					if(result[r][0] != 1){
						D.insert(D.end(),route.begin(),route.begin()+result[r][0]-1);
					}
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
	//
	// inter-route Or-opt (# of nodes = 4~6)
	//
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
	//
	// inter-route Cross-exchange (# of nodes = 4~6)
	//
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = solution.routes_table;
	for(int i = 0;i < solution.route_num;i++){
		vector<int> route1 = rt[i];
		// determine the length of segments: [4,min(6,n)]
		const int MIN_LEN = 4;
		const int MAX_LEN = 6;
		int NUM_OF_NODE1;
		if(rt[i].size() > MIN_LEN){

			if(rt[i].size() == 4){ NUM_OF_NODE1 = 4; }
			else if(rt[i].size() == 5){ NUM_OF_NODE1 = rand() % 2 + MIN_LEN; }
			else{ NUM_OF_NODE1 = rand() % 3 + MIN_LEN; }

			for(int j = 0;j < rt[i].size()-NUM_OF_NODE1+1;j++){
				vector<int> seg1;
				seg1.assign(route1.begin()+j,route1.begin()+j+NUM_OF_NODE1);
				for(int k = 0;k < solution.route_num;k++){
					if(k == i)
						continue;
					int NUM_OF_NODE2;
					if(rt[k].size() > MIN_LEN){

						if(rt[k].size() == 4){ NUM_OF_NODE2 = 4; }
						else if(rt[k].size() == 5){ NUM_OF_NODE2 = rand() % 2 + MIN_LEN; }
						else{ NUM_OF_NODE2 = rand() % 3 + MIN_LEN; }

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
	//
	// inter-route iCross-exchange (# of nodes = 4~6)
	//
	vector<SolutionNode> sn_vec;
	vector< vector<int> > rt = solution.routes_table;
	for(int i = 0;i < rt.size();i++){
		vector<int> route1 = rt[i];
		// determine the length of segments: [4,min(6,n)]
		const int MIN_LEN = 4;
		const int MAX_LEN = 6;
		int NUM_OF_NODE1;
		if(rt[i].size() > MIN_LEN){

			if(rt[i].size() == 4){ NUM_OF_NODE1 = 4; }
			else if(rt[i].size() == 5){ NUM_OF_NODE1 = rand() % 2 + MIN_LEN; }
			else{ NUM_OF_NODE1 = rand() % 3 + MIN_LEN; }

			for(int j = 0;j < rt[i].size()-NUM_OF_NODE1+1;j++){
				vector<int> seg1;
				seg1.assign(route1.begin()+j,route1.begin()+j+NUM_OF_NODE1);
				for(int k = 0;k < rt.size();k++){
					if(k == i)
						continue;
					int NUM_OF_NODE2;
					if(rt[k].size() > MIN_LEN){

						if(rt[k].size() == 4){ NUM_OF_NODE2 = 4; }
						else if(rt[k].size() == 5){ NUM_OF_NODE2 = rand() % 2 + MIN_LEN; }
						else{ NUM_OF_NODE2 = rand() % 3 + MIN_LEN; }

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