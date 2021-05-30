#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <math.h>
#include <regex>
#include "rand_sa.h" // class SavingsAlgo
#include "gvns.h"
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
extern string DIR_PATH;
extern int total_postal_num;
extern int total_cust_num;
extern vector<int> cust_postal_num;
extern float T;
extern const float SPEED;
extern const float SERV_COST;
extern const int time_period;
extern const int MAX_POSTAL_NUM;
//
class ReadFile{
public:
	void run(){
		ifstream file;
		file.open(DIR_PATH+"sp1_result.txt");
		string str;
		// get w, get m_I, m_O, best rotation degree
		getline(file,str);
		w = stoi(str);
		getline(file,str);
		m_I = stoi(str);
		getline(file,str);
		m_O = stoi(str);
		getline(file,str);
		best_rot_deg = stoi(str);
		// get best 1st layer center
		getline(file,str);
		best_1st_center = get_node(str);
		// get best 2nd layer center
		for(int i = 0;i < m_I;i++){
			getline(file,str);
			Node tmp = get_node(str);
			best_2nd_center.push_back(tmp);
		}
		// get 1st district endpoints
		for(int i = 0;i < m_I;i++){
			getline(file,str);
			int num = stoi(str);
			vector<Node> tmp_vec;
			for(int j = 0;j < num;j++){
				getline(file,str);
				Node tmp_n = get_node(str);
				tmp_vec.push_back(tmp_n);
			}
			district_endpoints_1st.push_back(tmp_vec);
		}
		if(w == 1){
			// get 1st layer districted customer points
			for(int i = 0;i < m_I;i++){
				getline(file,str);
				int num = stoi(str);
				vector<Node> tmp_vec;
				for(int j = 0;j < num;j++){
					getline(file,str);
					Node tmp_n = get_node(str);
					tmp_vec.push_back(tmp_n);
				}
				district_customers_1st.push_back(tmp_vec);
			}
			// get 1st layer exchange points
			for(int i = 0;i < m_I;i++){
				getline(file,str);
				Node tmp = get_node(str);
				exch_points_1st.push_back(tmp);
			}
		}
		if(w == 2){
			// get best 3rd layer center
			for(int i = 0;i < m_I;i++){
				vector<Node> tmp_v;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					Node tmp = get_node(str);
					tmp_v.push_back(tmp);
				}
				best_3rd_center.push_back(tmp_v);
			}
			// get 2nd district endpoints
			for(int i = 0;i < m_I;i++){
				vector< vector<Node> > tmp_vv;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					int num = stoi(str);
					vector<Node> tmp_v;
					for(int k = 0;k < num;k++){
						getline(file,str);
						Node tmp_n = get_node(str);
						tmp_v.push_back(tmp_n);
					}
					tmp_vv.push_back(tmp_v);
				}
				district_endpoints_2nd.push_back(tmp_vv);
			}
			// get 2nd districted customer points
			for(int i = 0;i < m_I;i++){
				vector< vector<Node> > tmp_vv;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					int num = stoi(str);
					vector<Node> tmp_v;
					for(int k = 0;k < num;k++){
						getline(file,str);
						Node tmp_n = get_node(str);
						tmp_v.push_back(tmp_n);
					}
					tmp_vv.push_back(tmp_v);
				}
				district_customers_2nd.push_back(tmp_vv);
			}
			// get 1st layer exchange points
			for(int i = 0;i < m_I;i++){
				getline(file,str);
				Node tmp = get_node(str);
				exch_points_1st.push_back(tmp);
			}
			// get 1st layer exchange points
			// get best 3rd layer center
			for(int i = 0;i < m_I;i++){
				vector<Node> tmp_v;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					Node tmp = get_node(str);
					tmp_v.push_back(tmp);
				}
				exch_points_2nd.push_back(tmp_v);
			}
		}
		file.close();

		// read district number
		file.open("cust_postal_num.txt");
		getline(file,str);
		total_postal_num = stoi(str);
		getline(file,str);
		total_cust_num = stoi(str);
		for(int i = 0;i < total_cust_num;i++){
			getline(file,str);
			cust_postal_num.push_back(stoi(str));
		}
		file.close();
	}
	Node get_node(string str){
		int idx;
		Node tmp;
		idx = str.find(" ");
		tmp.x = stoi(str.substr(0,idx));
		tmp.y = stoi(str.substr(idx+1,str.size()-idx));
		//tmp.show();
		return tmp;
	}
};

class TopoSolution{
private:
	vector<int> fixed_courier_num_list;
	vector<vector<Node>> distr_cust_points;
	vector<vector<vector<Node>>> time_cust_points;
	vector<vector<vector<vector<float>>>> cust_dist;
	vector<vector<int>> postal_nums;
	vector<vector<int>> time_cust_nums;
	vector<Node> exch_points;
	vector<int> peak_time;
	vector<vector<SolutionNode>> init_solution;
	vector<vector<SolutionNode>> same_courier_num_solution;
	vector<vector<SolutionNode>> balance_solution;
	vector<vector<SolutionNode>> familiarity_solution;
	float DELTA_1; // for balancing workload
	float DELTA_2; // for increasing familiarity
public:
	//
	// Constructor & Deconstructor
	//
	TopoSolution(vector<vector<Node>> _distr_cust_points, vector<Node> _exch_points,float DELTA_1_,float DELTA_2_){
		distr_cust_points = _distr_cust_points;
		exch_points = _exch_points;
		DELTA_1 = DELTA_1_;
		DELTA_2 = DELTA_2_;
		for(int i = 0;i < m_I;i++){
			vector<SolutionNode> vec;
			for(int j = 0;j < time_period;j++){
				SolutionNode sn;
				vec.push_back(sn);
			}
			init_solution.push_back(vec);
		}
	}
	~TopoSolution(){}

	//
	// Member function
	//
	vector<int>  split(const string& str,const string& delim) { 
		vector<int> res;
		if("" == str) return  res;
		
		string strs = str + delim; 
		size_t pos;
		size_t size = strs.size();
	 
		for (int i = 0; i < size; ++i) {
			pos = strs.find(delim, i);
			if( pos < size) { 
				string s = strs.substr(i, pos - i);
				res.push_back(stoi(s));
				i = pos + delim.size() - 1;
			}
		}
		return res;	
	}
	void readInputFile(){
		ifstream file;
		// Read table 1: get customer number in each distirct & time period
		file.open(DIR_PATH+"table1.txt");
		while(file){
			string str;
			getline(file,str);
			vector<int> tmp_v = split(str," ");
			time_cust_nums.push_back(tmp_v);
		}
		file.close();
		// Read table 2: get postal number
		file.open(DIR_PATH+"table2.txt");
		while(file){
			string str;
			getline(file,str);
			vector<int> tmp_v = split(str," ");
			postal_nums.push_back(tmp_v);
		}
		file.close();
	}
	void splitCustByTime(){
		for(int i = 0;i < m_I;i++){
			vector<int> &num_list = time_cust_nums[i];
			vector<Node> &cur_cust_points = distr_cust_points[i];
			int base = 0;
			vector<vector<Node>> cust_per_time;
			for(int j = 0;j < num_list.size();j++){
				vector<Node> tmp;
				tmp.assign(cur_cust_points.begin()+base,cur_cust_points.begin()+num_list[j]+base);
				base += num_list[j];
				cust_per_time.push_back(tmp);
			}
			time_cust_points.push_back(cust_per_time);
		}
	}
	void calcDist(){
		//
		// Calculate the distance between customer points in the same district & time period
		// NOTICE: Since dist[j][j] always = 0, I use this to be the distance between exchange point & Node j
		//
		for(int i = 0;i < m_I;i++){
			vector<vector<vector<float>>> time_cust_dist;
			Node exch_point = exch_points[i];
			for(int j = 0;j < time_period;j++){
				vector<Node> &cur_list = time_cust_points[i][j];
				vector<vector<float>> mat(cur_list.size(), vector<float>(cur_list.size(),0));
				for(int k = 0;k < cur_list.size();k++){
					for(int l = 0;l < cur_list.size();l++){
						if(k != l){
							float dist = sqrt(pow(cur_list[k].x-cur_list[l].x,2) + pow(cur_list[k].y-cur_list[l].y,2));
							mat[k][l] = dist;
						}
						else{
							float dist = sqrt(pow(cur_list[k].x-exch_point.x,2) + pow(cur_list[k].y-exch_point.y,2));
							mat[k][l] = dist;
						}
					}
				}
				time_cust_dist.push_back(mat);
			}
			cust_dist.push_back(time_cust_dist);
		}
	}
	SolutionNode doSavingAlgo(vector<Node> cust_points, Node exch_point, vector<vector<float>> dist_matrix){
		SavingsAlgo sa(cust_points,exch_point,dist_matrix);
		sa.run();
		return sa.get_solution();
	}
	void getInitSolution(bool isTest){
		for(int i = 0;i < m_I;i++){
			for(int j = 0;j < time_period;j++){
				cout << "--District " << i << ", Time period " << j << "---" << endl;
				SolutionNode sn = doSavingAlgo(time_cust_points[i][j],exch_points[i],cust_dist[i][j]);
				sn.show();
				GVNS gvns(sn,time_cust_points[i][j],exch_points[i],cust_dist[i][j]);
				if(!isTest)
					gvns.run();
				gvns.solution.show();
				init_solution[i][j] = gvns.solution;
			}
		}
		cout << "---- initial solution ok ----" << endl << endl;
	}
	void useSameNumCourier(vector<int> fixed_courier_num_list_){
		fixed_courier_num_list = fixed_courier_num_list_;
		same_courier_num_solution = init_solution;
		for(int i = 0;i < m_I;i++){
			for(int j = 0;j < time_period;j++){
				cout << "--District " << i << ", Time period " << j << "---" << endl;
				if(init_solution[i][j].routes_table.size() >= fixed_courier_num_list[i]){
					continue;
				}
				GVNS gvns(init_solution[i][j],time_cust_points[i][j],exch_points[i],cust_dist[i][j],fixed_courier_num_list[i]);
				gvns.run();
				gvns.solution.show();
				same_courier_num_solution[i][j] = gvns.solution;
			}
		}
		cout << "---- use same courier number solution ok ----" << endl << endl;
	}
	vector<float> getCorierRoutingTotalTime(int distr_num,int fixed_courier_num){
		vector<float> total_time_vec(fixed_courier_num,0);
		for(int i = 0;i < time_period;i++){
			SolutionNode sn = same_courier_num_solution[distr_num][i];
			for(int t = 0;t < sn.routes_time.size();t++){
				total_time_vec[t] += sn.routes_time[t];
			}
		}
		return total_time_vec;
	}
	vector<int> getCorierRoutingTimeOrder(int distr_num,int fixed_courier_num){
		vector<float> total_time_vec = getCorierRoutingTotalTime(distr_num,fixed_courier_num);
		// lambda compare function, sort to be ascending
		auto func = [total_time_vec](int a,int b) { 
			return total_time_vec[a] < total_time_vec[b]; 
		};
		vector<int> vec(fixed_courier_num);
		// set value in range, begin from 0
		iota(vec.begin(), vec.end(), 0);
		sort(vec.begin(),vec.end(),func);
		return vec;
	}
	vector<SolutionNode> getNeighbors(SolutionNode sn, vector<int> fast,vector<int> slow, int type,vector<vector<float>> dist_table){
		vector<SolutionNode> sn_vec;
		sn_vec.push_back(sn);
		vector< vector<int> > rt = sn.routes_table;
		vector<float> routes_time = sn.routes_time;;
		int route_num = rt.size();
		// Shift(1,0)
		if(type == 0){
			for(int i = 0;i < slow.size();i++){
				vector<int> route = rt[slow[i]];
				int route_len = route.size();
				for(int j = 0;j < route_len;j++){
					vector<int> route_i = route;
					int n = route[j];
					route_i.erase(route_i.begin()+j);
					for(int k = 0;k < fast.size();k++){
						// do shift if slow route really take more time than fast route
						// do shif if routes are not the same
						if(slow[i] == fast[k] || routes_time[fast[k]] > routes_time[slow[i]]) 
							continue;
						for(int l = 0;l <= rt[fast[k]].size();l++){
							vector<int> route_k = rt[fast[k]];
							route_k.insert(route_k.begin()+l,n);
							vector< vector<int> > tmp_rt = rt;
							tmp_rt[fast[k]] = route_k;
							if(route_i.size() > 0){
								tmp_rt[slow[i]] = route_i;
							}
							else{
								tmp_rt.erase(tmp_rt.begin()+slow[i]);
							}
							SolutionNode tmp_sn(tmp_rt,dist_table);
							sn_vec.push_back(tmp_sn);
						}
					}
				}
			}
		}	
		// Shift(2,0)
		else if(type == 1){
			for(int i = 0;i < slow.size();i++){
				vector<int> route = rt[slow[i]];
				int route_len = route.size();
				if(route_len < 2)
					continue;
				for(int j = 0;j < route_len-1;j++){
					vector<int> route_i = route;
					int n1 = route[j];
					int n2 = route[j+1];
					route_i.erase(route_i.begin()+j,route_i.begin()+j+2);
					for(int k = 0;k < fast.size();k++){
						// do shift if slow route really take more time than fast route
						// do shif if routes are not the same
						if(slow[i] == fast[k] || routes_time[fast[k]] > routes_time[slow[i]]) 
							continue;
						for(int l = 0;l <= rt[fast[k]].size();l++){
							vector<int> route_k = rt[fast[k]];
							route_k.insert(route_k.begin()+l,n1);
							route_k.insert(route_k.begin()+l+1,n2);
							vector< vector<int> > tmp_rt = rt;
							tmp_rt[fast[k]] = route_k;
							if(route_i.size() > 0){
								tmp_rt[slow[i]] = route_i;
							}
							else{
								tmp_rt.erase(tmp_rt.begin()+slow[i]);
							}
							SolutionNode tmp_sn(tmp_rt,dist_table);
							sn_vec.push_back(tmp_sn);
						}
					}
				}
			}
		}
		else if(type == 2){
			int NUM_OF_NODE = 2;
			// intra-route 2-opt
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
						SolutionNode tmp_sn(tmp_rt,dist_table);
						sn_vec.push_back(tmp_sn);
					}
				}
			}
		}
		else if(type == 3){
			// intra-route Or-opt
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
						SolutionNode tmp_sn(tmp_rt,dist_table);
						sn_vec.push_back(tmp_sn);
					}
				}
			}
		}
		else{
			// intra-route 3-opt
			for(int i = 0;i < route_num;i++){
				int NUM_OF_NODE = 3;
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
						SolutionNode tmp_sn1(tmp_rt,dist_table);
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
						SolutionNode tmp_sn2(tmp_rt,dist_table);
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
						SolutionNode tmp_sn3(tmp_rt,dist_table);
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
						SolutionNode tmp_sn4(tmp_rt,dist_table);
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
						SolutionNode tmp_sn5(tmp_rt,dist_table);
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
						SolutionNode tmp_sn6(tmp_rt,dist_table);
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
						SolutionNode tmp_sn7(tmp_rt,dist_table);
						sn_vec.push_back(tmp_sn7);
					}
				}
			}
		}
		return sn_vec;
	}
	void comb_go(int offset, int k, vector<int> &combination, vector<int> people,vector< vector<int> > &result) {
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
	bool check_comb(vector<int> vec){
		for(int i = 0;i < vec.size()-1;i++){
			if(vec[i+1]-vec[i]<=1)
				return false;
		}
		return true;
	}
	SolutionNode findBalanceNeighbor(vector<SolutionNode> &neighbors,vector<int> &fast,vector<int> &slow,float time_limit){
		float gap = FLT_MAX;
		SolutionNode result;
		for(int i = 0;i < neighbors.size();i++){
			SolutionNode &cur_sn = neighbors[i];
			// Lambda function: check if illegal
			bool illegal = [=](SolutionNode sn){
				for(int n = 0;n < sn.routes_time.size();n++){
					if(sn.routes_time[n] > T || sn.total_time > time_limit*(1.0+DELTA_1))
						return true;
				}
				return false;
			}(cur_sn);
			if(illegal) continue;
			// calculate fast couriers' total time
			float fast_total_time = 0;
			for(int j = 0;j < fast.size();j++){
				fast_total_time += cur_sn.routes_time[fast[j]];
			}
			// calculate slow couriers' total time
			float slow_total_time = 0;
			for(int j = 0;j < slow.size();j++){
				slow_total_time += cur_sn.routes_time[slow[j]];
			}
			if(abs(fast_total_time-slow_total_time) < gap){
				gap = abs(fast_total_time-slow_total_time);
				result = cur_sn;
			}
		}
		return result;
	}
	SolutionNode doBalanceVND(int region,int period,vector<int> &fast,vector<int> &slow){
		SolutionNode sn = same_courier_num_solution[region][period];
		vector<vector<float>> dist_table = cust_dist[region][period];
		float time_limit = sn.total_time;
		float gap = 0.0;
		for(int j = 0;j < fast.size();j++){
			gap -= sn.routes_time[fast[j]];
		}
		float slow_total_time = 0;
		for(int j = 0;j < slow.size();j++){
			gap += sn.routes_time[slow[j]];
		}
		gap = abs(gap);
		for(int type = 0;type < 5;type++){
			vector<SolutionNode> neighbors = getNeighbors(sn,fast,slow,type,dist_table);
			SolutionNode balance_neighbor = findBalanceNeighbor(neighbors,fast,slow,time_limit);
			// calculate gap of cur sn & balance neighbor
			float balance_gap;

			// To prevent Balance neighbor = NULL
			if(balance_neighbor.routes_table.size() != 0)
				balance_gap = 0.0;
			else{
				cout << "ERROR: Exceed time limit in workload balance part" << endl;
				exit(0);
			}
			//
			for(int j = 0;j < fast.size();j++){
				balance_gap -= balance_neighbor.routes_time[fast[j]];
			}
			for(int j = 0;j < slow.size();j++){
				balance_gap += balance_neighbor.routes_time[slow[j]];
			}
			if(abs(balance_gap) < gap){
				gap = abs(balance_gap);
				sn = balance_neighbor;
				type = 0;
			}
		}
		return sn;
	}
	void balanceWorkload(int FIRST_SHORT, int LAST_LONG){
		balance_solution = same_courier_num_solution;
		for(int i = 0; i < m_I;i++){
		//for(int i = 0; i < 1;i++){
			// Error detection
			if((FIRST_SHORT+LAST_LONG) > fixed_courier_num_list[i]){
				cout << "Workload balancing failed: FIRST_SHORT or LAST_LONG too big" << endl;
				continue;
			}
			
			vector<int> vec = getCorierRoutingTimeOrder(i,fixed_courier_num_list[i]);
			// get first m fast routing couriers & last n slow routing couriers
			vector<int> fast_courier_num;
			vector<int> slow_courier_num;
			fast_courier_num.assign(vec.begin(),vec.begin()+FIRST_SHORT);
			slow_courier_num.assign(vec.end()-LAST_LONG,vec.end());
			//
			vector<float> total_time_vec = getCorierRoutingTotalTime(i,fixed_courier_num_list[i]);

			// show total time of each courier
			// for(int j = 0;j < total_time_vec.size();j++){
			// 	cout << total_time_vec[j] << " ";
			// }
			// cout << endl;

			for(int j = 0;j < time_period;j++){
				cout << "---District " << i << ", Time period " << j << "---" << endl;
				SolutionNode sn = doBalanceVND(i,j,fast_courier_num,slow_courier_num);
				sn.show();
				balance_solution[i][j] = sn;
			}
			// total_time_vec = getCorierRoutingTotalTime(i,fixed_courier_num_list[i]);
			// for(int j = 0;j < total_time_vec.size();j++){
			// 	cout << total_time_vec[j] << " ";
			// }
			// cout << endl;
		}
		cout << "---- Balance workload end ----" << endl << endl;
	}
	void showFamiliarityScore(vector<SolutionNode> distr_solution,vector<int> cust_postal_nums,int fixed_courier_num){
		vector<vector<int>> score_matrix(fixed_courier_num,vector<int>(6,0));
		int cnt = 0;
		// 
		for(int i = 0;i < distr_solution.size();i++){
			SolutionNode &sn = distr_solution[i];
			for(int r = 0;r < fixed_courier_num;r++){
				vector<int> &route = sn.routes_table[r];
				for(int j = 0;j < route.size();j++){
					score_matrix[r][cust_postal_nums[route[j]]]++;
					if(score_matrix[r][cust_postal_nums[route[j]]] == 1)
						cnt++;
				}
			}
		}
		// show postal distribution of courier
		for(int i = 0;i < fixed_courier_num;i++){
			for(int j = 0;j < MAX_POSTAL_NUM;j++){
				cout << score_matrix[i][j] << " ";
			}
			cout << endl;
		}
		cout << cnt << endl;
		cout << endl << endl;
	}
	vector<vector<int>> getFamiliarityScoreMatrix(vector<SolutionNode> distr_solution,vector<int> cust_postal_nums,int fixed_courier_num){
		vector<vector<int>> score_matrix(fixed_courier_num,vector<int>(MAX_POSTAL_NUM,0));
		// 
		for(int i = 0;i < distr_solution.size();i++){
			SolutionNode sn = distr_solution[i];
			for(int r = 0;r < fixed_courier_num;r++){
				vector<int> route = sn.routes_table[r];
				for(int j = 0;j < route.size();j++){
					score_matrix[r][cust_postal_nums[route[j]]]++;
				}
			}
		}
		return score_matrix;
	}
	float getWorkload(SolutionNode sn){
		float min = FLT_MAX;
		float max = 0.0;
		for(int i = 0;i < sn.routes_time.size();i++){
			if(sn.routes_time[i] < min)
				min = sn.routes_time[i];
			if(sn.routes_time[i] > max)
				max = sn.routes_time[i];
		}
		return abs(max - min);
	}
	vector<SolutionNode> getNeighborsFam(SolutionNode sn, int courier_num, int postal_num, int type, vector<vector<float>> dist_table,vector<int> period_postal_nums){
		vector<SolutionNode> sn_vec;
		sn_vec.push_back(sn);
		vector<vector<int>> rt = sn.routes_table;
		int route_num = rt.size();
		
		if(type == 0){
			//
			// TODO: shift(1,0)
			//
			//cout << "size: " << period_postal_nums.size() << endl;
			for(int i = 0;i < rt[courier_num].size();i++){
				//cout << "Compare: " << period_postal_nums[rt[courier_num][i]] << " " << postal_num << endl;
				//cout << "cust num: " << rt[courier_num][i] << endl;
				if(period_postal_nums[rt[courier_num][i]] == postal_num){
					int cust_n = rt[courier_num][i];
					vector<int> tmp1 = rt[courier_num];
					tmp1.erase(tmp1.begin()+i);
					for(int j = 0;j < route_num;j++){
						if(j == courier_num)
							continue;
						vector<int> route = rt[j];
						for(int k = 0;k <= route.size();k++){
							vector<int> tmp2 = route;
							tmp2.insert(tmp2.begin()+k,cust_n);
							//
							vector<vector<int>> tmp_rt = rt;
							tmp_rt[courier_num] = tmp1;
							tmp_rt[j] = tmp2;
							SolutionNode tmp_sn(tmp_rt,dist_table);
							sn_vec.push_back(tmp_sn);
						}
					}
				}
			}
		}
		else if(type == 1){
			int NUM_OF_NODE = 2;
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
						SolutionNode tmp_sn(tmp_rt,dist_table);
						sn_vec.push_back(tmp_sn);
					}
				}
			}
		}
		else if(type == 2){
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
						SolutionNode tmp_sn(tmp_rt,dist_table);
						sn_vec.push_back(tmp_sn);
					}
				}
			}
		}
		else if(type == 3){
			int NUM_OF_NODE = 3;
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
						SolutionNode tmp_sn1(tmp_rt,dist_table);
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
						SolutionNode tmp_sn2(tmp_rt,dist_table);
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
						SolutionNode tmp_sn3(tmp_rt,dist_table);
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
						SolutionNode tmp_sn4(tmp_rt,dist_table);
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
						SolutionNode tmp_sn5(tmp_rt,dist_table);
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
						SolutionNode tmp_sn6(tmp_rt,dist_table);
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
						SolutionNode tmp_sn7(tmp_rt,dist_table);
						sn_vec.push_back(tmp_sn7);
					}
				}
			}
		}
		return sn_vec;
	}
	bool exceedTimeLimit(SolutionNode sn){
		for(int i = 0;i < sn.routes_time.size();i++){
			if(sn.routes_time[i] > T)
				return true;
		}
		return false;
	}
	SolutionNode findBestNeighbor(vector<SolutionNode> neighbors,float time_limit,float workload){
		float cur_time = FLT_MAX;
		SolutionNode sn = neighbors[0];
		//cout << "neighbors size: "  << neighbors.size() << endl;
		if(neighbors.size() >= 2){
			for(int i = 1;i < neighbors.size();i++){
				// if(exceedTimeLimit(neighbors[i])){
				// 	continue;
				// }
				float wl = getWorkload(neighbors[i]);
				//if(neighbors[i].total_time < cur_time && (neighbors[i].total_time < time_limit*(1.0+DELTA_1)) && (wl < (workload+DELTA_2))){
				if(neighbors[i].total_time < cur_time){
					sn = neighbors[i];
					cur_time = neighbors[i].total_time;
				}
			}
		}
		return sn;
	}
	void doVNDII(int region,int courier_num,int postal_num,int shift_cnt){
		//
		// Shift operation
		//
		// get neighbors
		for(int t = 0;t < time_period;t++){
			cout << "---District " << region << ", Time period " << t << "---" <<endl;
			SolutionNode sn = familiarity_solution[region][t];
			sn.show();
			float time_limit = same_courier_num_solution[region][t].total_time;
			vector<vector<float>> dist_table = cust_dist[region][t];
			float workload = getWorkload(sn);
			// do not need to do VND
			if(sn.routes_table[courier_num].size() <= 1)
				continue;
			
			vector<int> period_postal_nums;
			if(t != 0)
				period_postal_nums.assign(postal_nums[region].begin()+time_cust_nums[region][t-1],postal_nums[region].begin()+time_cust_nums[region][t-1]+time_cust_nums[region][t]);
			else
				period_postal_nums.assign(postal_nums[region].begin(),postal_nums[region].begin()+time_cust_nums[region][t]);
			for(int i = 0;i < period_postal_nums.size();i++)
				cout << period_postal_nums[i] << " ";
			cout << endl;
			// cout << "Total: " << time_cust_nums[region][t] << endl;
			for(int cnt = 0;cnt < shift_cnt;cnt++){
				for(int type = 0;type < 4;type++){
					vector<SolutionNode> neighbors = getNeighborsFam(sn,courier_num,postal_num,type,dist_table,period_postal_nums);
					SolutionNode best_neighbor = findBestNeighbor(neighbors,time_limit,workload);
					if(type != 0 && best_neighbor.total_time < sn.total_time){
						type = 1;
						sn = best_neighbor;
					}
					else if(type == 0){
						sn = best_neighbor;
				}
				familiarity_solution[region][t] = sn;
				sn.show();
				// do not need to do VND
				if(sn.routes_table[courier_num].size() <= 1)
					break; 
				}
			}
		}
	}
	void doFamiliarityVND(int region,int VISIT_LOW_BOUND){
		// vector<SolutionNode> sn = init_solution[region];
		vector<SolutionNode> distr_solution = familiarity_solution[region];
		vector<int> distr_postal_nums = postal_nums[region];
		int fixed_courier_num = fixed_courier_num_list[region];
		showFamiliarityScore(distr_solution,distr_postal_nums,fixed_courier_num);
		
		//
		vector<vector<int>> score_matrix = getFamiliarityScoreMatrix(distr_solution,distr_postal_nums,fixed_courier_num);
		for(int i = 0;i < fixed_courier_num;i++){
			for(int j = 0;j < MAX_POSTAL_NUM;j++){
				if(score_matrix[i][j] >= 1 && score_matrix[i][j] <= VISIT_LOW_BOUND){
					doVNDII(region,i,j,score_matrix[i][j]);
					score_matrix = getFamiliarityScoreMatrix(familiarity_solution[region],distr_postal_nums,fixed_courier_num);
				}
			}
		}
		showFamiliarityScore(familiarity_solution[region],distr_postal_nums,fixed_courier_num);
	}
	void increaseFamiliarity(int VISIT_LOW_BOUND){
		familiarity_solution = balance_solution;
		for(int i = 0;i < m_I;i++){
			doFamiliarityVND(i,VISIT_LOW_BOUND);
		}
	}
};


