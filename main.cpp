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
const float H = 2.0;
float T; // unit: hr
const float SPEED = 40000; // unit: km/hr
const int time_period = 10;

//
int main(){

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
	cout << "Customer numbers: " << district_customers_1st[0].size() << endl;
	cout << "Customer numbers(in one time period): " << district_customers_1st[0].size() / time_period << endl;
	cout << "T: " << T << endl; 
	cout << "Time period: " << time_period << endl;

	//
	// Split customer points into different time period
 	//
 	Node cur_exch_point = exch_points_1st[0];
 	vector<Node> district_customers = district_customers_1st[0];
 	vector< vector<Node> > cur_customers; // different time period's customer points
 	const int CUS_NUM_PERIOD = district_customers_1st[0].size() / time_period;
 	int peak_time1 = 4;
 	int peak_time2 = 5;
 	int nonpeak_time1 = 2;
 	int nonpeak_time2 = 7;
 	for(int i = 0;i < time_period;i++){
 		vector<Node> tmp;
 		if(i < time_period -1){
 			if(i == peak_time1 || i == peak_time2){
 				for(int j = 0;j < CUS_NUM_PERIOD*2;j++){
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
	// do randomized savings algo.
	//
	SavingsAlgo sa(cur_customers[7],cur_exch_point);
	sa.run();


	//
	// do GVNS
	//
	srand(time(NULL));
	SolutionNode sn = sa.get_solution();
	GVNS gvns(sn,cur_customers[7],cur_exch_point);
	gvns.run();
 	// for(int i = 0;i < 10;i++){
 	// 	cout << cur_customers[i].size() << endl;
 	// }
 	// for(int i = 0; i < time_period;i++){
 	// 	cout << "\n-------- Time period " << i << " --------"<< endl;
 	// 	//
		// // do randomized savings algo.
		// //
		// SavingsAlgo sa(cur_customers[i],cur_exch_point);
		// sa.run();


		// //
		// // do GVNS
		// //
		// srand(time(NULL));
		// SolutionNode sn = sa.get_solution();
		// GVNS gvns(sn,cur_customers[i],cur_exch_point);
		// gvns.run();
		// cout << "route num: " << gvns.solution.route_num << endl;
 	// }
	// main function's return value
	return 0;
}