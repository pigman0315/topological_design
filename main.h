#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>
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
extern float T;
extern const float H;
extern const float SPEED;
extern const float SERV_COST;
extern const int time_period;
extern const int MAX_POSTAL_NUM;
//
class ReadFile{
public:
	void init(){
		best_2nd_center.clear();
		district_endpoints_1st.clear();
		district_customers_1st.clear();
		exch_points_1st.clear();
		int total_postal_num = MAX_POSTAL_NUM;
		visit_time_vec.clear();
	}
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
			file.close();
		}
		if(w == 2){
			m_I = m_O+1; // not a good operation (workaround for w = 2)
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
			file.close();
		}
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
		file.open(DIR_PATH+"c_m_l.txt");
		while(file){
			string str;
			getline(file,str);
			vector<int> tmp_v = split(str," ");
			time_cust_nums.push_back(tmp_v);
		}
		file.close();
		// Read table 2: get postal number
		file.open(DIR_PATH+"c_w.txt");
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
	void useSameNumCourier(vector<int> peak_time){
		same_courier_num_solution = init_solution;
		// build fixed_courier_num_list(private member of this class)
		for(int i = 0;i < m_I;i++){
			int max = 0;
			for(int j = 0;j < time_period;j++){
				if(find(peak_time.begin(),peak_time.end(),j) == peak_time.end() 
					&& init_solution[i][j].routes_table.size() > max)
				{
					max = init_solution[i][j].routes_table.size();
				}
			}
			fixed_courier_num_list.push_back(max);
		}

		// use same courier number
		for(int i = 0;i < m_I;i++){
			for(int j = 0;j < time_period;j++){
				cout << "--District " << i << ", Time period " << j << "---" << endl;
				if(init_solution[i][j].routes_table.size() >= fixed_courier_num_list[i]){
					same_courier_num_solution[i][j].show();
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
	vector<float> getCourierRoutingTotalTime(int distr_num,int fixed_courier_num){
		vector<float> total_time_vec(fixed_courier_num,0);
		for(int i = 0;i < time_period;i++){
			SolutionNode sn = same_courier_num_solution[distr_num][i];
			for(int t = 0;t < sn.routes_time.size();t++){
				total_time_vec[t] += sn.routes_time[t];
			}
		}
		return total_time_vec;
	}
	vector<vector<int>> getCourierRoutesTimeOrder(int distr_num,int fixed_courier_num){
		vector<vector<int>> result;
		for(int i = 0;i < fixed_courier_num;i++){
			vector<float> times;
			for(int t = 0 ;t < time_period;t++){
				SolutionNode sn = same_courier_num_solution[distr_num][t];
				times.push_back(sn.routes_time[i]);
			}

			// lambda compare function, sort ascendingly
			auto func = [times](int a,int b) { 
				return times[a] < times[b]; 
			};

			// sorting
			vector<int> vec(time_period);
			iota(vec.begin(), vec.end(), 0); // set value in range,e.g. 0 1 2 3 4 5...
			sort(vec.begin(), vec.end(), func);

			// store result
			result.push_back(vec);
		}
		return result;
	}
	vector<int> getCourierRoutingTimeOrder(int distr_num,int fixed_courier_num){
		vector<float> total_time_vec = getCourierRoutingTotalTime(distr_num,fixed_courier_num);
		// lambda compare function, sort to be ascending
		auto func = [total_time_vec](int a,int b) { 
			return total_time_vec[a] < total_time_vec[b]; 
		};

		// sorting
		vector<int> vec(fixed_courier_num);
		iota(vec.begin(), vec.end(), 0); // set value in range,e.g. 0 1 2 3 4 5...
		sort(vec.begin(),vec.end(),func);
		return vec;
	}
	vector<SolutionNode> getNeighborsBalance(SolutionNode sn, int fast, int slow, int type,vector<vector<float>> dist_table){
		vector<SolutionNode> sn_vec;
		sn_vec.push_back(sn);
		vector< vector<int> > rt = sn.routes_table;
		vector<float> routes_time = sn.routes_time;;
		int route_num = rt.size();
		// Shift(1,0)
		if(type == 0){
			vector<int> route = rt[slow];
			int route_len = route.size();
			for(int j = 0;j < route_len;j++){
				vector<int> route_i = route;
				int n = route[j];
				route_i.erase(route_i.begin()+j);
				for(int l = 0;l <= rt[fast].size();l++){
					vector<int> route_k = rt[fast];
					route_k.insert(route_k.begin()+l,n);
					vector< vector<int> > tmp_rt = rt;
					//
					tmp_rt[slow] = route_i;
					tmp_rt[fast] = route_k;
					SolutionNode tmp_sn(tmp_rt,dist_table);
					sn_vec.push_back(tmp_sn);
				}
			}
		}	
		// Shift(2,0)
		else if(type == 1){
			vector<int> route = rt[slow];
			int route_len = route.size();
			if(route_len < 2)
				return sn_vec;
			for(int j = 0;j < route_len-1;j++){
				vector<int> route_i = route;
				int n1 = route[j];
				int n2 = route[j+1];
				route_i.erase(route_i.begin()+j,route_i.begin()+j+2);
				for(int l = 0;l <= rt[fast].size();l++){
					vector<int> route_k = rt[fast];
					route_k.insert(route_k.begin()+l,n1);
					route_k.insert(route_k.begin()+l+1,n2);
					vector< vector<int> > tmp_rt = rt;
					tmp_rt[fast] = route_k;
					tmp_rt[slow] = route_i;
					SolutionNode tmp_sn(tmp_rt,dist_table);
					sn_vec.push_back(tmp_sn);
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
	SolutionNode findBalanceNeighbor(vector<SolutionNode> &neighbors,int fast,int slow,float time_limit,float base_time){
		float gap = FLT_MAX;
		SolutionNode result;
		for(int i = 0;i < neighbors.size();i++){
			SolutionNode &cur_sn = neighbors[i];
			// Lambda function: check if illegal
			bool illegal = [=](SolutionNode sn){
				for(int n = 0;n < sn.routes_time.size();n++){
					if(sn.routes_time[n] > T || exceedTotalTimeLimit(sn.total_time,base_time,time_limit))
						return true;
				}
				return false;
			}(cur_sn);
			if(illegal)
				continue;
			// calculate fast couriers' total time
			float fast_total_time = 0;
			fast_total_time += cur_sn.routes_time[fast];
			// calculate slow couriers' total time
			float slow_total_time = 0;
			slow_total_time += cur_sn.routes_time[slow];
			//
			if(abs(fast_total_time-slow_total_time) < gap){
				gap = abs(fast_total_time-slow_total_time);
				result = cur_sn;
			}
		}
		return result;
	}
	SolutionNode doBalanceVND(int region,int period, int fast, int slow){
		SolutionNode sn = balance_solution[region][period];
		vector<vector<float>> dist_table = cust_dist[region][period];
		float base_time = sn.total_time;
		float time_limit = getTimelimit(balance_solution);
		float gap = 0.0;
		gap -= sn.routes_time[fast];
		gap += sn.routes_time[slow];
		gap = abs(gap);
		cout << "initial gap: " << gap << endl;
		for(int type = 0;type < 5;type++){
			vector<SolutionNode> neighbors = getNeighborsBalance(sn,fast,slow,type,dist_table);
			cout << "neighbor size = " << neighbors.size() << endl;
			SolutionNode balance_neighbor = findBalanceNeighbor(neighbors,fast,slow,time_limit,base_time);
			// calculate gap of cur sn & balance neighbor
			float balance_gap;

			// To prevent Balance neighbor = NULL
			if(balance_neighbor.routes_table.size() != 0)
				balance_gap = 0.0;
			else{
				cout << "ERROR: Exceed time limit in workload balance part(region = " << region << " & time period = " << period << ")" << endl;
				return same_courier_num_solution[region][period];
				
			}
			//
			balance_gap -= balance_neighbor.routes_time[fast];
			balance_gap += balance_neighbor.routes_time[slow];
			cout << "gap: " << balance_gap << endl;
			if(abs(balance_gap) < gap){
				gap = abs(balance_gap);
				sn = balance_neighbor;
				type = 0;
			}
		}
		return sn;
	}
	void checkMatchWorkload(vector<int> fast_courier_num, vector<int> slow_courier_num, 
		vector<vector<int>> routes_time_order, int FIRST_SHORT_R,int LAST_LONG_R,int region,
		int time_period_)
	{
		for(int i = 0;i < fast_courier_num.size();i++){
			vector<int> rto_fast = routes_time_order[fast_courier_num[i]];
			vector<int> v1;
			v1.assign(rto_fast.begin(),rto_fast.begin()+FIRST_SHORT_R);
			auto f1 = find(v1.begin(),v1.end(),time_period_);
			for(int j = 0;j < slow_courier_num.size();j++){
				vector<int> rto_slow = routes_time_order[slow_courier_num[j]];
				vector<int> v2;
				v2.assign(rto_slow.end()-LAST_LONG_R,rto_slow.end());
				//
				auto f2 = find(v2.begin(),v2.end(),time_period_);
				//
				if(f1 != v1.end() && f2 != v2.end()){
					cout << "- before - " << endl;
					balance_solution[region][time_period_].show();
					SolutionNode sn = doBalanceVND(region,time_period_,fast_courier_num[i],slow_courier_num[j]);
					cout << "- after - " << endl;
					sn.show();
					balance_solution[region][time_period_] = sn;
				}
				else{
					cout << "ERROR: didn't match workload time in time period: " << time_period_ << endl;
				}
			}
		}
	}
	void balanceWorkload(int FIRST_SHORT, int LAST_LONG, int FIRST_SHORT_R, int LAST_LONG_R){
		balance_solution = same_courier_num_solution;
		
		// in each district
		for(int i = 0; i < m_I;i++){
			// Error detection
			if((FIRST_SHORT+LAST_LONG) > fixed_courier_num_list[i]){
				cout << "Workload balancing failed: FIRST_SHORT or LAST_LONG too big" << endl;
				continue;
			}
			
			vector<int> vec = getCourierRoutingTimeOrder(i,fixed_courier_num_list[i]);
			// get first m fast routing couriers & last n slow routing couriers
			vector<int> fast_courier_num;
			vector<int> slow_courier_num;
			fast_courier_num.assign(vec.begin(),vec.begin()+FIRST_SHORT);
			slow_courier_num.assign(vec.end()-LAST_LONG,vec.end());
			
			//show total time of each courier
			vector<float> total_time_vec = getCourierRoutingTotalTime(i,fixed_courier_num_list[i]);
			for(int j = 0;j < total_time_vec.size();j++){
				cout << total_time_vec[j] << " ";
			}
			cout << endl;
			
			// get each courier's routes time order
			cout << "- route time order -" << endl;
			vector<vector<int>>  routes_time_order = getCourierRoutesTimeOrder(i,fixed_courier_num_list[i]);
			for(int a = 0;a < routes_time_order.size();a++){
				for(int b = 0;b < routes_time_order[a].size();b++){
					cout << routes_time_order[a][b] << " ";
				}
				cout << endl;
			}
			//
			for(int j = 0;j < time_period;j++){
				cout << "---District " << i << ", Time period " << j << "---" << endl;
				
				// check if last slow routes match first fast routes
				checkMatchWorkload(fast_courier_num,slow_courier_num,routes_time_order,FIRST_SHORT_R,LAST_LONG_R,i,j);
			}

			//show total time of each courier
			total_time_vec = getCourierRoutingTotalTime(i,fixed_courier_num_list[i]);
			for(int j = 0;j < total_time_vec.size();j++){
				cout << total_time_vec[j] << " ";
			}
			cout << endl;
		}
		cout << "---- Balance workload end ----" << endl << endl;
	}
	void showFamiliarityScore(vector<SolutionNode> distr_solution,int fixed_courier_num, int region){
		vector<vector<int>> score_matrix(fixed_courier_num,vector<int>(MAX_POSTAL_NUM,0));
		// 
		for(int i = 0;i < distr_solution.size();i++){
			SolutionNode sn = distr_solution[i];

			// make postal number vector in certain time period and region
			vector<int> period_postal_nums;
			if(i != 0)
				period_postal_nums.assign(postal_nums[region].begin()+time_cust_nums[region][i-1],postal_nums[region].begin()+time_cust_nums[region][i-1]+time_cust_nums[region][i]);
			else
				period_postal_nums.assign(postal_nums[region].begin(),postal_nums[region].begin()+time_cust_nums[region][i]);
			
			//
			for(int r = 0;r < fixed_courier_num;r++){
				vector<int> route = sn.routes_table[r];
				for(int j = 0;j < route.size();j++){
					score_matrix[r][period_postal_nums[route[j]]]++;
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
	}
	int getTotalFamiliarityScore(vector<vector<SolutionNode>> solution){
		int count = 0;
		//
		for(int a = 0;a < solution.size();a++){
			vector<SolutionNode> distr_solution = solution[a];
			vector<vector<int>> score_matrix(fixed_courier_num_list[a],vector<int>(MAX_POSTAL_NUM,0));
			int fixed_courier_num = fixed_courier_num_list[a];
			for(int i = 0;i < distr_solution.size();i++){
				SolutionNode sn = distr_solution[i];

				// make postal number vector in certain time period and region
				vector<int> period_postal_nums;
				if(i != 0)
					period_postal_nums.assign(postal_nums[a].begin()+time_cust_nums[a][i-1],postal_nums[a].begin()+time_cust_nums[a][i-1]+time_cust_nums[a][i]);
				else
					period_postal_nums.assign(postal_nums[a].begin(),postal_nums[a].begin()+time_cust_nums[a][i]);
				
				//
				for(int r = 0;r < fixed_courier_num;r++){
					vector<int> route = sn.routes_table[r];
					for(int j = 0;j < route.size();j++){
						score_matrix[r][period_postal_nums[route[j]]]++;
						if(score_matrix[r][period_postal_nums[route[j]]] == 1)
							count++;
					}
				}
			}
		}
		return count;
	}
	vector<vector<int>> getFamiliarityScore(vector<SolutionNode> distr_solution,int fixed_courier_num, int region){
		vector<vector<int>> score_matrix(fixed_courier_num,vector<int>(MAX_POSTAL_NUM,0));
		// 
		for(int i = 0;i < distr_solution.size();i++){
			SolutionNode sn = distr_solution[i];

			// make postal number vector in certain time period and region
			vector<int> period_postal_nums;
			if(i != 0)
				period_postal_nums.assign(postal_nums[region].begin()+time_cust_nums[region][i-1],postal_nums[region].begin()+time_cust_nums[region][i-1]+time_cust_nums[region][i]);
			else
				period_postal_nums.assign(postal_nums[region].begin(),postal_nums[region].begin()+time_cust_nums[region][i]);
			
			//
			for(int r = 0;r < fixed_courier_num;r++){
				vector<int> route = sn.routes_table[r];
				for(int j = 0;j < route.size();j++){
					score_matrix[r][period_postal_nums[route[j]]]++;
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
	vector<SolutionNode> getInitNeigborsFam(SolutionNode sn, int target_courier_num, int target_postal_num, vector<vector<float>> dist_table,vector<int> period_postal_nums){
		// preparation
		vector<SolutionNode> sn_vec;
		vector<vector<int>> rt = sn.routes_table;
		int route_num = rt.size();

		// Find and delete all infamiliar nodes from original route
		vector<int> infam_nodes;
		vector<int> tmp_route;
		for(int i = 0;i < rt[target_courier_num].size();i++){
			if(period_postal_nums[rt[target_courier_num][i]] == target_postal_num){
				infam_nodes.push_back(rt[target_courier_num][i]);
			}
			else
				tmp_route.push_back(rt[target_courier_num][i]);
		}
		rt[target_courier_num] = tmp_route;

		// Insert all infamiliar nodes into other routes
		vector<vector<vector<int>>> rts;
		rts.push_back(rt);
		for(int i = 0;i < infam_nodes.size();i++){
			vector<vector<vector<int>>> tmp_rts;
			for(int j = 0;j < rts.size();j++){
				vector<vector<int>> tmp_rt = rts[j];
				for(int k = 0;k < tmp_rt.size();k++){
					if(k == target_courier_num)
						continue;
					for(int l = 0;l <= tmp_rt[k].size();l++){
						vector<vector<int>> tmp_rt2 = tmp_rt;
						tmp_rt2[k].insert(tmp_rt2[k].begin()+l,infam_nodes[i]);
						tmp_rts.push_back(tmp_rt2);
					}
				}
			}
			rts = tmp_rts;
		}
		// Build solution node based on rts
		cout << rts.size() << endl;
		for(int i = 0;i < rts.size();i++){
			SolutionNode tmp_sn(rts[i],dist_table);
			sn_vec.push_back(tmp_sn);
		}
		//
		return sn_vec;
	}
	vector<SolutionNode> getNeighborsFam(SolutionNode sn, int type, vector<vector<float>> dist_table){
		// preparation
		vector<SolutionNode> sn_vec;
		vector<vector<int>> rt = sn.routes_table;
		int route_num = rt.size();
		
		//
		if(type == 2){
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
	float getTimelimit(vector<vector<SolutionNode>> solution){
		float time_limit = getTotalTime(same_courier_num_solution)*(1.0+DELTA_1) - getTotalTime(solution);
		if(time_limit > 0)
			return time_limit;
		else
			return 0.0;
	}
	bool exceedRouteTimeLimit(SolutionNode sn){
		for(int i = 0;i < sn.routes_time.size();i++){
			if(sn.routes_time[i] > T)
				return true;
		}
		return false;
	}
	bool exceedTotalTimeLimit(float cur_time, float base_time, float time_limit){
		if(cur_time < base_time)
			return false;
		if(cur_time - base_time < time_limit)
			return false;
		else
			return true;
	}
	void getWorkloadMaxMin(vector<vector<SolutionNode>> solution, float &min, float &max){
		min = FLT_MAX;
		max = 0;
		for(int i = 0;i < m_I;i++){
			vector<float> wl(fixed_courier_num_list[i],0.0);
			vector<SolutionNode> &s_vec = solution[i];
			for(int t = 0;t < s_vec.size();t++){
				SolutionNode &s = s_vec[t];
				for(int c = 0;c < fixed_courier_num_list[i];c++){
					wl[c] += s.routes_time[c];
				}
			}
			for(int j = 0;j < wl.size();j++){
				if(wl[j] < min)
					min = wl[j];
				if(wl[j] > max)
					max = wl[j];
			}
		}	
	}
	bool exceedWorkloadLimit(SolutionNode sn, int region, int time_period_){
		vector<vector<SolutionNode>> solution = balance_solution;
		solution[region][time_period_] = sn;
		//
		float min_o, max_o, min_n, max_n;
		getWorkloadMaxMin(balance_solution,min_o,max_o);
		getWorkloadMaxMin(solution,min_n,max_n);
		if((max_n - min_n) < ((max_o - min_o)+DELTA_2))
			return false;
		else
			return true;
	}
	SolutionNode findLegalNeighbor(vector<SolutionNode> neighbors,float base_time,float time_limit, int region, int time_period_){
		float cur_time = FLT_MAX;
		SolutionNode sn;
		for(int i = 0;i < neighbors.size();i++){
			if(exceedRouteTimeLimit(neighbors[i]) || 
				exceedWorkloadLimit(neighbors[i], region, time_period_) || 
				exceedTotalTimeLimit(neighbors[i].total_time, base_time, time_limit)
			) continue;
			//
			if(neighbors[i].total_time < cur_time){
				sn = neighbors[i];
				cur_time = neighbors[i].total_time;
			}
		}
		return sn;
	}
	void doVNDII(int region,int courier_num,int postal_num){
		
		for(int t = 0;t < time_period;t++){
			//cout << "--- District " << region << ", Time period " << t << "---" <<endl;
			SolutionNode sn = familiarity_solution[region][t];
			float base_time = same_courier_num_solution[region][t].total_time; // for delta_1
			float time_limit = getTimelimit(familiarity_solution);
			vector<vector<float>> dist_table = cust_dist[region][t];
			float base_workload = getWorkload(sn); // for delta_2
			
			// do not need to do VND
			if(sn.routes_table[courier_num].size() == 0)
				continue;
			
			// make postal number vector in certain time period and region
			vector<int> period_postal_nums;
			if(t != 0)
				period_postal_nums.assign(postal_nums[region].begin()+time_cust_nums[region][t-1],postal_nums[region].begin()+time_cust_nums[region][t-1]+time_cust_nums[region][t]);
			else
				period_postal_nums.assign(postal_nums[region].begin(),postal_nums[region].begin()+time_cust_nums[region][t]);
			// cout << "Period postal number: ";
			// for(int i = 0;i < period_postal_nums.size();i++)
			// 	cout << period_postal_nums[i] << " ";
			// cout << endl;

			// Get inital neighbors of familiarity
			vector<SolutionNode> init_neighbors =  getInitNeigborsFam(sn, courier_num, postal_num, dist_table, period_postal_nums);
			
			// Improve inital neighbors of familiarity
			for(int i = 0;i < init_neighbors.size();i++){
				SolutionNode tmp_sn = init_neighbors[i];
				for(int type = 2;type < 5;type++){
					vector<SolutionNode> neighbors = getNeighborsFam(tmp_sn,type,dist_table);
					SolutionNode legal_neighbor = findLegalNeighbor(neighbors,base_time,time_limit,region,t);
					//
					if(legal_neighbor.routes_table.size() == 0)
						break;
					//
					if(legal_neighbor.total_time < tmp_sn.total_time){
						tmp_sn = legal_neighbor;
						type = 2;
					}
				}
				init_neighbors[i] = tmp_sn;
			}
			// Find a best initial neighbors
			SolutionNode best_sn;
			float best_total_time = FLT_MAX;
			for(int i = 0;i < init_neighbors.size();i++){
				if(exceedRouteTimeLimit(init_neighbors[i]) || 
					exceedWorkloadLimit(init_neighbors[i], region, t) || 
					exceedTotalTimeLimit(init_neighbors[i].total_time, base_time, time_limit)
				) 
					continue;
				//
				if(init_neighbors[i].total_time < best_total_time){
					best_sn = init_neighbors[i];
					best_total_time = init_neighbors[i].total_time;
				}
			}
			if(best_sn.routes_table.size() == 0){
				cout << "ERROR: cannot increase familiarity in district " << region << endl;
				familiarity_solution[region] = balance_solution[region];
				return;
			}
			familiarity_solution[region][t] = best_sn;
		}
	}
	void doFamiliarityVND(int region,int VISIT_LOW_BOUND){
		int fixed_courier_num = fixed_courier_num_list[region];
		cout << "--- Before: ---" << endl;
		showFamiliarityScore(familiarity_solution[region],fixed_courier_num,region);
		//
		vector<vector<int>> score_matrix = getFamiliarityScore(familiarity_solution[region],fixed_courier_num,region);
		for(int i = 0;i < fixed_courier_num;i++){
			for(int j = 0;j < MAX_POSTAL_NUM;j++){
				if(score_matrix[i][j] >= 1 && score_matrix[i][j] < VISIT_LOW_BOUND){
					doVNDII(region,i,j);
					score_matrix = getFamiliarityScore(familiarity_solution[region],fixed_courier_num,region);
				}
			}
		}
		cout << "--- After: ---" << endl;
		showFamiliarityScore(familiarity_solution[region],fixed_courier_num,region);
	}
	void increaseFamiliarity(int VISIT_LOW_BOUND){
		if(balance_solution.size() == 0)
			familiarity_solution = same_courier_num_solution;
		else
			familiarity_solution = balance_solution;
		for(int i = 0;i < m_I;i++){
			doFamiliarityVND(i,VISIT_LOW_BOUND);
		}
		cout << "---- Increasing familiarity end ----" << endl;
	}
	float getTotalTime(vector<vector<SolutionNode>> solution){
		float time = 0.0;
		for(int i = 0;i < solution.size();i++){
			for(int j = 0;j < solution[i].size();j++){
				time += solution[i][j].total_time;
			}
		}
		return time;
	}
	void showWorkload(vector<vector<SolutionNode>> solution){
		cout << "Workload:" << endl;
		for(int i = 0;i < m_I;i++){
			cout << "\tDistrct " << (i+1) << ": ";
			vector<float> wl(fixed_courier_num_list[i],0.0);
			vector<SolutionNode> &s_vec = solution[i];
			for(int t = 0;t < s_vec.size();t++){
				SolutionNode &s = s_vec[t];
				for(int c = 0;c < fixed_courier_num_list[i];c++){
					wl[c] += s.routes_time[c];
				}
			} 
			for(int j = 0;j < wl.size();j++){
				cout << wl[j]*60.0 << "(mins)" << " ";
			}
			cout << endl;
		}
			
	}
	void showFinalResult(){
		//
		// Routes
		//
		cout << "\n=== Routes ===" << endl;
		for(int r = 0;r < familiarity_solution.size();r++){
			for(int t = 0;t < familiarity_solution[r].size();t++){
				cout << "-- District " << r << ", Time period " << t << "---" << endl; 
				familiarity_solution[r][t].show();
			}
		}
		
		//
		// Parameters
		//
		cout << "\n=== Parameters ===" << endl;
		cout << "H = " << H*60.0 << "(mins)" << endl;
		cout << "T = " << T*60.0 << "(mins)" << endl;
		cout << "Speed = " << SPEED/1000.0 << "(km/hr)" << endl;
		cout << "Service cost = " << SERV_COST*60.0 << "(mins)" << endl;
		cout << "Time period = " << time_period << endl;
		cout << "Max postal number = " << MAX_POSTAL_NUM << endl;
		cout << "Delta 1 = " << DELTA_1 << endl;
		cout << "Delta 2 = " << DELTA_2*60.0 << endl;
		//
		// Statistics
		//
		cout << "\n=== Statistics ===" << endl;
		float total_time = 0.0;
		float min,max;
		int fam_score;

		total_time = getTotalTime(same_courier_num_solution);
		cout << "\n--- Object 1 ---" << endl;
		cout << "Total time = " << total_time*60 << "(mins)" << endl;
		showWorkload(same_courier_num_solution);

		total_time = getTotalTime(balance_solution);
		cout << "\n--- Object 2(Workload balance) ---" << endl;
		cout << "Total time = " << total_time*60 << "(mins)" << endl;
		getWorkloadMaxMin(balance_solution,min,max);
		cout << "Workload difference = " << (max - min)*60.0 << endl;
		showWorkload(balance_solution);

		total_time = getTotalTime(familiarity_solution);
		cout << "\n--- Object 3(Familiarity) ---" << endl;
		cout << "Total time = " << total_time*60 << "(mins)" << endl;
		getWorkloadMaxMin(familiarity_solution,min,max);
		cout << "Workload difference =  " << (max - min)*60.0 << endl;
		showWorkload(familiarity_solution);
		fam_score = getTotalFamiliarityScore(familiarity_solution);
		cout << "Familiarity score = " << fam_score << endl;
 	}
};


