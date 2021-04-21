#include "main.h" // class ReadFile
// Global variables
int w, m_I, m_O, best_rot_deg;
Node best_1st_center;
vector<Node> best_2nd_center;
vector< vector<Node> > best_3rd_center;
vector< vector<Node> > district_endpoints_1st;
vector< vector< vector<Node> > > district_endpoints_2nd;
vector< vector<Node> > district_customers_1st;
vector< vector< vector<Node> > > district_customers_2nd;
vector<Node> exch_points_1st;
vector< vector<Node> > exch_points_2nd;
int total_postal_num;
int total_cust_num;
vector<int> cust_postal_num;
vector< vector<int> > visit_time_vec;
const float H = 2.0;
float T; // unit: hr
const float SPEED = 40000; // unit: km/hr
const float SERV_COST = 0.025; // 1.5 min = 0.025 hr
const int time_period = 3;
const float DIST_RATE = 10.0; // to simulate real distance from Euclidean distance of two points
const int MAX_POSTAL_NUM = 6;
class TopoSolution1{
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
	int DELTA_1; // for balancing workload
	int DELTA_2; // for increasing familiarity
public:	
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
		file.open("table1.txt");
		while(file){
			string str;
			getline(file,str);
			vector<int> tmp_v = split(str," ");
			time_cust_nums.push_back(tmp_v);
		}
		file.close();
		// Read table 2: get postal number
		file.open("table2.txt");
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
	void getInitSolution(){
		for(int i = 0;i < m_I;i++){
			for(int j = 0;j < time_period;j++){
				cout << "--District " << i << ", Time period " << j << "---" << endl;
				// cout << "--Map--" << endl;
				// for(int a = 0;a < cust_dist[i][j].size();a++){
				// 	for(int b = 0;b < cust_dist[i][j].size();b++){
				// 		cout << cust_dist[i][j][a][b] << " ";
				// 	}
				// 	cout << endl;
				// }
				// cout << "--Map--" << endl;
				SolutionNode sn = doSavingAlgo(time_cust_points[i][j],exch_points[i],cust_dist[i][j]);
				sn.show();
				GVNS gvns(sn,time_cust_points[i][j],exch_points[i],cust_dist[i][j]);
				//gvns.run();
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
				if(init_solution[i][j].routes_table.size() == fixed_courier_num_list[i]){
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
			cout << sn.routes_time.size() << endl;
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
	vector<SolutionNode> getNeighbors(SolutionNode &sn, vector<int> &fast,vector<int> &slow, int type,vector<vector<float>> dist_table){
		vector<SolutionNode> sn_vec;
		sn_vec.push_back(sn);
		vector< vector<int> > &rt = sn.routes_table;
		vector<float> &routes_time = sn.routes_time;;
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
					if(sn.routes_time[n] > T || sn.total_time < time_limit*(1+DELTA_1))
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
		vector<vector<float>> &dist_table = cust_dist[region][period];
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
		sn.show();
		for(int type = 0;type < 5;type++){
			vector<SolutionNode> neighbors = getNeighbors(sn,fast,slow,type,dist_table);
			SolutionNode balance_neighbor = findBalanceNeighbor(neighbors,fast,slow,time_limit);
			// calculate gap of cur sn & balance neighbor
			float balance_gap = 0.0;
			for(int j = 0;j < fast.size();j++){
				balance_gap -= balance_neighbor.routes_time[fast[j]];
			}
			for(int j = 0;j < slow.size();j++){
				balance_gap += balance_neighbor.routes_time[slow[j]];
			}
			if(abs(balance_gap) < gap){
				gap = abs(balance_gap);
				sn = balance_neighbor;
				sn.show();
				cout << sn.total_time << ", ";
				cout << gap << endl;
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
				return;
			}
			
			vector<int> vec = getCorierRoutingTimeOrder(i,fixed_courier_num_list[i]);
			// get first m fast routing couriers & last n slow routing couriers
			vector<int> fast_courier_num;
			vector<int> slow_courier_num;
			fast_courier_num.assign(vec.begin(),vec.begin()+FIRST_SHORT);
			slow_courier_num.assign(vec.end()-LAST_LONG,vec.end());
			//
			vector<float> total_time_vec = getCorierRoutingTotalTime(i,fixed_courier_num_list[i]);
			for(int j = 0;j < total_time_vec.size();j++){
				cout << total_time_vec[j] << " ";
			}
			cout << endl;
			for(int j = 0;j < time_period;j++){
				cout << "--District " << i << ", Time period " << j << "---" << endl;
				SolutionNode sn = doBalanceVND(i,j,fast_courier_num,slow_courier_num);
				balance_solution[i][j] = sn;
			}
			total_time_vec = getCorierRoutingTotalTime(i,fixed_courier_num_list[i]);
			for(int j = 0;j < total_time_vec.size();j++){
				cout << total_time_vec[j] << " ";
			}
			cout << endl;
		}
		cout << "---- Balance workload end ----" << endl << endl;
	}
	int getFamiliarityScore(vector<SolutionNode> distr_solution,vector<int> cust_postal_nums,int fixed_courier_num){
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
		return cnt;
	}
	vector<vector<int>> getFamiliarityScoreMatrix(vector<SolutionNode> distr_solution,vector<int> cust_postal_nums,int fixed_courier_num){
		vector<vector<int>> score_matrix(fixed_courier_num,vector<int>(6,0));
		// 
		for(int i = 0;i < distr_solution.size();i++){
			SolutionNode &sn = distr_solution[i];
			for(int r = 0;r < fixed_courier_num;r++){
				vector<int> &route = sn.routes_table[r];
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
			for(int i = 0;i < rt[courier_num].size();i++){
				if(period_postal_nums[rt[courier_num][i]] == postal_num){
					int cust_n = rt[courier_num][i];
					vector<int> tmp1 = rt[courier_num];
					tmp1.erase(tmp1.begin()+i);
					for(int j = 0;j < route_num;j++){
						if(j == courier_num)
							continue;
						vector<int> route = rt[j];
						for(int k = 0;k < route.size();k++){
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
		else if(type == 2){
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
		else if(type == 3){
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
		else if(type == 4){
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
	SolutionNode findBestNeighbor(vector<SolutionNode> neighbors,float time_limit,float workload){
		float cur_time = FLT_MAX;
		SolutionNode sn = neighbors[0];
		if(neighbors.size() >= 2){
			for(int i = neighbors.size()-1;i>=1;i--){
				float wl = getWorkload(neighbors[i]);
				if(neighbors[i].total_time < cur_time && neighbors[i].total_time < time_limit*(1+DELTA_1) && wl < (workload+DELTA_2)){
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
			SolutionNode &sn = familiarity_solution[region][t];
			float time_limit = same_courier_num_solution[region][t].total_time;
			vector<vector<float>> dist_table = cust_dist[region][t];
			float workload = getWorkload(sn);
			// do not need to do VND
			if(sn.routes_table[courier_num].size() <= 1)
				return;
			//
			vector<int> period_postal_nums;
			if(t != 0)
				period_postal_nums.assign(postal_nums[region].begin()+time_cust_nums[region][t-1],postal_nums[region].begin()+time_cust_nums[region][t-1]+time_cust_nums[region][t]);
			else
				period_postal_nums.assign(postal_nums[region].begin(),postal_nums[region].begin()+time_cust_nums[region][t]);
			//
			while(shift_cnt > 0){
				for(int type = 0;type < 4;type++){
					vector<SolutionNode> neighbors = getNeighborsFam(sn,courier_num,postal_num,type,dist_table,period_postal_nums);
					SolutionNode best_neighbor = findBestNeighbor(neighbors,time_limit,workload);
					if(type != 0 && best_neighbor.total_time != sn.total_time){
						type = 1;
						sn = best_neighbor;
						sn.show();
					}
					else if(type == 0){
						sn = best_neighbor;
						sn.show();
					}
				}
				shift_cnt--;
			}
		}
	}
	vector<SolutionNode> doFamiliarityVND(int region,int VISIT_LOW_BOUND){
		// vector<SolutionNode> sn = init_solution[region];
		vector<SolutionNode> distr_solution = familiarity_solution[region];
		vector<int> distr_postal_nums = postal_nums[region];
		int fixed_courier_num = fixed_courier_num_list[region];
		int familiarity_score = getFamiliarityScore(distr_solution,distr_postal_nums,fixed_courier_num);
		
		//
		vector<vector<int>> score_matrix = getFamiliarityScoreMatrix(distr_solution,distr_postal_nums,fixed_courier_num);
		for(int i = 0;i < fixed_courier_num;i++){
			for(int j = 0;j < MAX_POSTAL_NUM;j++){
				if(score_matrix[i][j] >= 1 && score_matrix[i][j] <= VISIT_LOW_BOUND){
					doVNDII(region,i,j,score_matrix[i][j]);
					score_matrix = getFamiliarityScoreMatrix(distr_solution,distr_postal_nums,fixed_courier_num);
					familiarity_score = getFamiliarityScore(distr_solution,distr_postal_nums,fixed_courier_num);
				}
			}
		}
		return distr_solution;
	}
	void increaseFamiliarity(int VISIT_LOW_BOUND){
		// familiarity_solution = balance_solution;
		// balance_solution = same_courier_num_solution;
		familiarity_solution = balance_solution;
		// fixed_courier_num_list = {3,3,3};
		for(int i = 0;i < m_I;i++){
			familiarity_solution[i] = doFamiliarityVND(i,VISIT_LOW_BOUND);
		}
	}
	TopoSolution1(vector<vector<Node>> _distr_cust_points, vector<Node> _exch_points,int DELTA_1_,int DELTA_2_){
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
	~TopoSolution1(){}
};
//
int main(){
	srand(time(NULL));

	//
	// Read file 
 	//
	ReadFile rf;
	rf.run();
	T = H / (0.5+m_I+2*(w-1)*m_O);

	//
	//
	//
	TopoSolution1 tp1(district_customers_1st,exch_points_1st,0.2,1.0);
	tp1.readInputFile();
	tp1.splitCustByTime();
	tp1.calcDist();
	tp1.getInitSolution();
	tp1.useSameNumCourier({3,3,3});
	tp1.balanceWorkload(1,1);
	tp1.increaseFamiliarity(4);
	//
	// outside for loop
	//
	// int cur_cnt = 0; // for postal number
	// for(int d = 0;d < m_I;d++){
	// 	//
	// 	// Show info 
	//  	//
	// 	cout << "----------------------------District " << d << " ----------------------------\n";
	// 	cout << "Customer numbers(total): " << total_cust_num << endl;
	// 	cout << "Customer numbers(one region): " << district_customers_1st[d].size() << endl;
	// 	cout << "Customer numbers(one region & one time period): " << district_customers_1st[d].size() / time_period << endl;
	// 	cout << "T: " << T << endl; 
	// 	cout << "Time period: " << time_period << endl;
	// 	cout << "total postal num: " << total_postal_num << endl;
	
	// 	//
	// 	// Split customer points into different time period
	//  	//
	//  	Node cur_exch_point = exch_points_1st[d];
	//  	vector<Node> district_customers = district_customers_1st[d];
	//  	vector< vector<Node> > cur_customers; // different time period's customer points
	//  	const int CUS_NUM_PERIOD = district_customers_1st[d].size() / time_period;
	//  	const int PEAK_NUM = 1;
	//  	int peak_time1 = -1;
	//  	int peak_time2 = -1;
	//  	int nonpeak_time1 = -1;
	//  	int nonpeak_time2 = -1;
	//  	int owned_courier_num;
	//  	for(int i = 0;i < time_period;i++){
	//  		vector<Node> tmp;
	//  		if(i < time_period - 1){
	//  			if(i == peak_time1 || i == peak_time2){
	//  				for(int j = 0;j < CUS_NUM_PERIOD*1.2;j++){
	// 		 			if(i*CUS_NUM_PERIOD+j < district_customers.size()){
	// 		 				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
	// 		 			}
	// 		 		}
	//  			}
	//  			else if(i == nonpeak_time1 || i == nonpeak_time2){
	//  				for(int j = 0;j < CUS_NUM_PERIOD*0.8;j++){
	// 		 			if(i*CUS_NUM_PERIOD+j < district_customers.size()){
	// 		 				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
	// 		 			}
	// 		 		}
	//  			}
	//  			else{
	//  				for(int j = 0;j < CUS_NUM_PERIOD;j++){
	// 		 			if(i*CUS_NUM_PERIOD+j < district_customers.size()){
	// 		 				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
	// 		 			}
	// 		 		}
	//  			}
	//  		}
	//  		else{
	//  			for(int j = 0;i*CUS_NUM_PERIOD+j < district_customers.size();j++){
	//  				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
	//  			}
	//  		}
	//  		cur_customers.push_back(tmp);
	//  	}

	//  	//
	// 	// Calculate travel time between each customer
	// 	// Based on the Euclidean Distance, we multiply a rate ranging between [1,1.5] with it
	//  	//
	//  	vector< vector< vector<float> > > cur_dist;
	//  	for(int i = 0;i < time_period;i++){
	//  		vector<Node> cus = cur_customers[i];
	//  		vector< vector<float> > tmp_dist;
	//  		int size = cus.size();
	//  		for(int j = 0;j < size;j++){
	//  			vector<float> tmp_v;
	//  			Node n1 = cus[j];
	//  			for(int k = 0;k < size;k++){
	//  				Node n2 = cus[k];
	//  				float dist, rate;
	//  				if(j == k){
	//  					dist = sqrt((cur_exch_point.x-n1.x)*(cur_exch_point.x-n1.x) + (cur_exch_point.y-n1.y)*(cur_exch_point.y-n1.y));
	//  					dist += SERV_COST*SPEED; // add service cost to distance;
	//  				}
	//  				else{
	//  					dist = sqrt((n1.x-n2.x)*(n1.x-n2.x) + (n1.y-n2.y)*(n1.y-n2.y));
	//  					dist += SERV_COST*SPEED*2; // add service cost to distance;
	//  				}
	//  				tmp_v.push_back(dist);
	//  			}
	//  			tmp_dist.push_back(tmp_v);
	//  		}
	//  		cur_dist.push_back(tmp_dist);
	//  	}

	//  	//
	//  	// Get improved solution
	//  	//
	//  	vector<float> time_initial;
	//  	vector<float> time_step2;
	//  	vector<float> time_step3;
	//  	vector<float> time_step45;
	//  	vector<float> time_step67;
	//  	cout << "--------------- Get improved solution ---------------" << endl;
	//  	vector<int> courier_num_vec;
	//  	vector<SolutionNode> solution_vec;
	//  	// for(int i = 0; i < time_period;i++){
	//  	for(int i = 0; i < time_period;i++){
	//  		cout << "-------- Time period " << i << " --------"<< endl;
	//  		//
	// 		// do randomized savings algo.
	// 		//
	// 		SavingsAlgo sa(cur_customers[i],cur_exch_point,cur_dist[i]);
	// 		sa.run();
	// 		sa.get_solution().show();
	// 		cout << "--- initial ok --- " << endl;

	// 		//
	// 		// do GVNS
	// 		//
	// 		SolutionNode sn = sa.get_solution();
	// 		time_initial.push_back(sn.total_time);
	// 		GVNS gvns(sn,cur_customers[i],cur_exch_point,cur_dist[i]);
	// 		// gvns.run();
	// 		gvns.solution.show();
	// 		cout << "--- step 2 ok --- " << endl;
	// 		time_step2.push_back(gvns.solution.total_time);
	// 		solution_vec.push_back(gvns.solution);
	// 		courier_num_vec.push_back(gvns.solution.route_num);
	//  	}

	//  	//
	//  	// Find the fixed number of routing couriers
	//  	//
	//  	sort(courier_num_vec.begin(),courier_num_vec.end());
	//  	owned_courier_num = courier_num_vec[courier_num_vec.size()-1-PEAK_NUM];
	//  	if(owned_courier_num < 2)
	//  		owned_courier_num = 2;
	//  	cout << endl << "Fixed number of routing couriers: " << owned_courier_num << endl;
	 	

	 // 	//
	 // 	// Use fixed number of owned routing couriers
	 // 	//
	 // 	cout << "--------------- Use fixed number of owned routing couriers ---------------" << endl;
		// for(int i = 0;i < time_period;i++){
		// 	cout << "-------- Time period " << i << " --------"<< endl;
		// 	//
		// 	// do fixed courier number GVNS
		// 	//
		// 	SolutionNode sn = solution_vec[i];
		// 	if(sn.routes_table.size() < owned_courier_num){
		// 		GVNS gvns(sn,cur_customers[i],cur_exch_point,owned_courier_num,cur_dist[i]);
		// 		gvns.run();
		// 		// gvns.solution.show();
		// 		cout << "--- step 3 ok --- " << endl;
		// 		//cout << gvns.solution.total_time << endl;
		// 		time_step3.push_back(gvns.solution.total_time);
		// 		solution_vec[i] = gvns.solution;
		// 	}
		// 	else{
		// 		cout << "--- step 3 ok --- " << endl;
		// 		time_step3.push_back(solution_vec[i].total_time);
		// 	}
		// 	// solution_vec[i].show();
		// }

		// //
		// // Define last n routing couriers & first m routing couriers
		// //
		// const int LAST_N = 2;
		// const int FIRST_M = 2;

		// //
		// // Balance the workload
		// //
		// cout << "--------------- Balance the workload ---------------" << endl;
		// for(int i = 0;i < time_period;i++){
		// 	cout << "-------- Time period " << i << " --------"<< endl;
		// 	//
		// 	// do fixed courier number GVNS
		// 	//
		// 	SolutionNode sn = solution_vec[i];
		// 	GVNS gvns(sn,cur_customers[i],cur_exch_point,owned_courier_num,cur_dist[i]);
		// 	gvns.do_work_balance(LAST_N, FIRST_M);
		// 	cout << "--- step 4,5 ok --- " << endl;
		// 	// gvns.solution.show();
		// 	//cout << gvns.solution.total_time << endl;
		// 	time_step45.push_back(gvns.solution.total_time);
		// 	solution_vec[i] = gvns.solution;
			
		// }

		// //
		// // Get visit time of each routing courier in all time
		// //
		// vector<int> postal_num;
		// for(int i = 0;i < owned_courier_num;i++){
		// 	vector<int> tmp(total_postal_num);
		// 	for(int j = 0;j < total_postal_num;j++){
		// 		tmp[j] = 0;
		// 	}
		// 	visit_time_vec.push_back(tmp);
			
		// }
		// for(int i = 0;i < time_period;i++){
		// 	vector< vector<int> > rt = solution_vec[i].routes_table;
		// 	int tmp_cnt = 0;
		// 	for(int j = 0;j < owned_courier_num;j++){
		// 		for(int k = 0;k < rt[j].size();k++){
		// 			int pn = cust_postal_num[rt[j][k]+cur_cnt];
		// 			visit_time_vec[j][pn]++;
		// 			tmp_cnt++;
		// 		}
		// 	}
		// 	for(int j = 0;j < rt.size();j++){
		// 		for(int k = 0;k < rt[j].size();k++){
		// 			int pn = cust_postal_num[rt[j][k]+cur_cnt];
		// 			postal_num.push_back(pn);
		// 		}
		// 	}
		// 	cur_cnt += tmp_cnt;
		// }

		// //
		// // Increase the familiarity
		// //
		// cout << "--------------- Increase the familiarity ---------------" << endl;
		// const int VISIT_LOW_BOUND = 3; // b in step 6,7
		// int count = 0;
		// for(int i = 0;i < time_period;i++){
		// 	SolutionNode sn = solution_vec[i];
		// 	// get postal number in the district
		// 	vector<int> distr_postal_num;
		// 	int n = sn.get_node_num();
		// 	for(int j = count;j < count+n;j++){
		// 		distr_postal_num.push_back(postal_num[j]);
		// 	}
		// 	//
		// 	cout << "-------- Time period " << i << " --------"<< endl;
		// 	//
		// 	// do fixed courier number GVNS
		// 	//
			
		// 	GVNS gvns(sn,cur_customers[i],cur_exch_point,owned_courier_num,cur_dist[i]);
		// 	gvns.read_postal_num(distr_postal_num);
		// 	cout << "--- step 6,7 ok --- " << endl;
		// 	// cout << "Table before: " << endl;
		// 	// gvns.show_familiar_table();
		// 	gvns.increase_familiarity(VISIT_LOW_BOUND);
		// 	// gvns.solution.show();
		// 	time_step67.push_back(gvns.solution.total_time);
		// 	// cout << "Table after: " << endl;
		// 	// gvns.show_familiar_table();
		// 	solution_vec[i] = gvns.solution;
		// }
		// for(int i = 0;i < time_period;i++){
		// 	cout << "---District: "<< d << ", Time period: " << i << " --- " << endl;
		// 	solution_vec[i].show();
		// }
		// //
		// // Show time
		// //
		// cout << "\n--------------- Show time of each step ---------------" << endl;
		// for(int i = 0;i < time_period;i++){
		// 	cout << "\n-------- Time period " << i << " --------"<< endl;
		// 	cout << "--- Initial time: " << time_initial[i] << " ---" << endl;
		// 	cout << "--- Step2 time: " << time_step2[i] << " ---" << endl;
		// 	cout << "--- Step3 time: " << time_step3[i] << " ---" << endl;
		// 	cout << "--- Step4,5 time: " << time_step45[i] << " ---" << endl;
		// 	cout << "--- Step6,7 time: " << time_step67[i] << " ---" << endl;
		// }
	// }

		
	// main function's return value
	return 0;
}