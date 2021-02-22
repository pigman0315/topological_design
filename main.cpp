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
const int time_period = 3;

//
int main(){
	srand(time(NULL));

	//
	// Read file 
 	//
	ReadFile rf;
	rf.run();


	//
	// Show info 
 	//
	T = H / (0.5+m_I+2*(w-1)*m_O);
	cout << "------------\n";
	cout << "Customer numbers(total): " << total_cust_num << endl;
	cout << "Customer numbers(one region): " << district_customers_1st[0].size() << endl;
	cout << "Customer numbers(one region & one time period): " << district_customers_1st[0].size() / time_period << endl;
	cout << "T: " << T << endl; 
	cout << "Time period: " << time_period << endl;
	cout << "total postal num: " << total_postal_num << endl;
	
	//
	// outside for loop
	//
	int cur_cnt = 0;

	//
	// Split customer points into different time period
 	//
 	Node cur_exch_point = exch_points_1st[0];
 	vector<Node> district_customers = district_customers_1st[0];
 	vector< vector<Node> > cur_customers; // different time period's customer points
 	const int CUS_NUM_PERIOD = district_customers_1st[0].size() / time_period;
 	const int PEAK_NUM = 2;
 	int peak_time1 = 0;
 	int peak_time2 = -1;
 	int nonpeak_time1 = 1;
 	int nonpeak_time2 = -1;
 	int owned_courier_num;
 	for(int i = 0;i < time_period;i++){
 		vector<Node> tmp;
 		if(i < time_period - 1){
 			if(i == peak_time1 || i == peak_time2){
 				for(int j = 0;j < CUS_NUM_PERIOD*1.5;j++){
		 			if(i*CUS_NUM_PERIOD+j < district_customers.size()){
		 				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
		 			}
		 		}
 			}
 			else if(i == nonpeak_time1 || i == nonpeak_time2){
 				for(int j = 0;j < CUS_NUM_PERIOD/2;j++){
		 			if(i*CUS_NUM_PERIOD+j < district_customers.size()){
		 				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
		 			}
		 		}
 			}
 			else{
 				for(int j = 0;j < CUS_NUM_PERIOD;j++){
		 			if(i*CUS_NUM_PERIOD+j < district_customers.size()){
		 				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
		 			}
		 		}
 			}
 		}
 		else{
 			for(int j = 0;i*CUS_NUM_PERIOD+j < district_customers.size();j++){
 				tmp.push_back(district_customers[i*CUS_NUM_PERIOD+j]);
 			}
 		}
 		cur_customers.push_back(tmp);
 	}


 	//
 	// Get improved solution
 	//
 	vector<float> time_initial;
 	vector<float> time_step2;
 	vector<float> time_step3;
 	vector<float> time_step45;
 	vector<float> time_step67;
 	cout << "\n--------------- Get improved solution ---------------" << endl;
 	vector<int> courier_num_vec;
 	vector<SolutionNode> solution_vec;
 	for(int i = 0; i < time_period;i++){
 		cout << "\n-------- Time period " << i << " --------"<< endl;
 		//
		// do randomized savings algo.
		//
		SavingsAlgo sa(cur_customers[i],cur_exch_point);
		sa.run();
		sa.get_solution().show();
		cout << "--- initial ok --- " << endl;

		//
		// do GVNS
		//
		SolutionNode sn = sa.get_solution();
		time_initial.push_back(sn.total_time);
		GVNS gvns(sn,cur_customers[i],cur_exch_point);
		gvns.run();
		gvns.solution.show();
		cout << "--- step 2 ok --- " << endl;
		time_step2.push_back(gvns.solution.total_time);
		solution_vec.push_back(gvns.solution);
		courier_num_vec.push_back(gvns.solution.route_num);
 	}

 	//
 	// Find the fixed number of routing couriers
 	//
 	sort(courier_num_vec.begin(),courier_num_vec.end());
 	// owned_courier_num = courier_num_vec[courier_num_vec.size()-1-PEAK_NUM];
 	owned_courier_num = 2;
 	cout << endl << "Fixed number of routing couriers: " << owned_courier_num << endl;
 	

 	//
 	// Use fixed number of owned routing couriers
 	//
 	cout << "\n--------------- Use fixed number of owned routing couriers ---------------" << endl;
	for(int i = 0;i < time_period;i++){
		cout << "\n-------- Time period " << i << " --------"<< endl;
		//
		// do fixed courier number GVNS
		//
		SolutionNode sn = solution_vec[i];
		if(sn.routes_table.size() < owned_courier_num){
			GVNS gvns(sn,cur_customers[i],cur_exch_point,owned_courier_num);
			gvns.run();
			gvns.solution.show();
			cout << "--- step 3 ok --- " << endl;
			//cout << gvns.solution.total_time << endl;
			time_step3.push_back(gvns.solution.total_time);
			solution_vec[i] = gvns.solution;
		}
		else{
			time_step3.push_back(solution_vec[i].total_time);
		}
	}

	//
	// Define last n routing couriers & first m routing couriers
	//
	const int LAST_N = 2;
	const int FIRST_M = 2;

	//
	// Balance the workload
	//
	cout << "\n--------------- Balance the workload ---------------" << endl;
	for(int i = 0;i < time_period;i++){
		cout << "\n-------- Time period " << i << " --------"<< endl;
		//
		// do fixed courier number GVNS
		//
		SolutionNode sn = solution_vec[i];
		GVNS gvns(sn,cur_customers[i],cur_exch_point,owned_courier_num);
		gvns.do_work_balance(LAST_N, FIRST_M);
		cout << "--- step 4,5 ok --- " << endl;
		// gvns.solution.show();
		//cout << gvns.solution.total_time << endl;
		time_step45.push_back(gvns.solution.total_time);
		solution_vec[i] = gvns.solution;
		
	}

	//
	// Get visit time of each routing courier in all time
	//
	vector<int> postal_num;
	for(int i = 0;i < owned_courier_num;i++){
		vector<int> tmp(total_postal_num);
		for(int j = 0;j < total_postal_num;j++){
			tmp[j] = 0;
		}
		visit_time_vec.push_back(tmp);
		
	}
	for(int i = 0;i < time_period;i++){
		vector< vector<int> > rt = solution_vec[i].routes_table;
		int tmp_cnt = 0;
		for(int j = 0;j < owned_courier_num;j++){
			for(int k = 0;k < rt[j].size();k++){
				int pn = cust_postal_num[rt[j][k]+cur_cnt];
				visit_time_vec[j][pn]++;
				tmp_cnt++;
			}
		}
		for(int j = 0;j < rt.size();j++){
			for(int k = 0;k < rt[j].size();k++){
				int pn = cust_postal_num[rt[j][k]+cur_cnt];
				postal_num.push_back(pn);
			}
		}
		cur_cnt += tmp_cnt;
	}

	//
	// Increase the familiarity
	//
	cout << "\n--------------- Increase the familiarity ---------------" << endl;
	const int VISIT_LOW_BOUND = 3; // b in step 6,7
	int count = 0;
	for(int i = 0;i < time_period;i++){
		SolutionNode sn = solution_vec[i];
		// get postal number in the district
		vector<int> distr_postal_num;
		int n = sn.get_node_num();
		for(int j = count;j < count+n;j++){
			distr_postal_num.push_back(postal_num[j]);
		}
		//
		cout << "\n-------- Time period " << i << " --------"<< endl;
		//
		// do fixed courier number GVNS
		//
		
		GVNS gvns(sn,cur_customers[i],cur_exch_point,owned_courier_num);
		gvns.read_postal_num(distr_postal_num);
		cout << "--- step 6,7 ok --- " << endl;
		cout << "score(before)" << endl;
		gvns.show_familiar_table();
		gvns.increase_familiarity(VISIT_LOW_BOUND);
		gvns.solution.show();
		time_step67.push_back(gvns.solution.total_time);
		cout << "score(after)" << endl;
		//gvns.show_familiar_table();
		solution_vec[i] = gvns.solution;
	}

	//
	// Show time
	//
	cout << "\n--------------- Show time of each step ---------------" << endl;
	for(int i = 0;i < time_period;i++){
		cout << "\n-------- Time period " << i << " --------"<< endl;
		cout << "--- Initial time: " << time_initial[i] << " ---" << endl;
		cout << "--- Step2 time: " << time_step2[i] << " ---" << endl;
		cout << "--- Step3 time: " << time_step3[i] << " ---" << endl;
		cout << "--- Step4,5 time: " << time_step45[i] << " ---" << endl;
		cout << "--- Step6,7 time: " << time_step67[i] << " ---" << endl;
	}
	// main function's return value
	return 0;
}