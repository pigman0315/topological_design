#include "main.h" // class ReadFile & TopoSolution1

//
// Global variables
//
int w, m_I, m_O, best_rot_deg;
Node best_1st_center;
vector<Node> best_2nd_center;
//vector< vector<Node> > best_3rd_center;
vector< vector<Node> > district_endpoints_1st;
//vector< vector< vector<Node> > > district_endpoints_2nd;
vector< vector<Node> > district_customers_1st;
//vector< vector< vector<Node> > > district_customers_2nd;
vector<Node> exch_points_1st;
// vector< vector<Node> > exch_points_2nd;
int total_postal_num;
vector< vector<int> > visit_time_vec;
string DIR_PATH;
const float H = 2.0;
float T; // unit: hr
const float SPEED = 200.0; // unit: m/hr
const float SERV_COST = 0.0167; // unit: hr, 0.0167 hr ~= 1 min
const int time_period = 3;
const int MAX_POSTAL_NUM = 6;
const float DELTA1 = 0.05;
const float DELTA2 = 0.1667; // unit: hr, 0.1667 hr ~= 10 min

//
// Main function
//
int main(){
	srand(time(NULL));
	//vector<string> dir_path_vec = {"0/","1/","2/","3/"}; // for 2-layer design
	vector<string> dir_path_vec = {"./"};
	for(int i = 0;i < dir_path_vec.size();i++){
		DIR_PATH = dir_path_vec[i];
		//
		// Read file 
	 	//
		ReadFile rf;
		rf.init();
		rf.run();
		T = H / (0.5+m_I+2*(w-1)*m_O);
		cout << "=== dir path: " << DIR_PATH << " ===" << endl;
		//
		// Get result of each process
		//
		TopoSolution tp(district_customers_1st,exch_points_1st,DELTA1,DELTA2); // Delta_1:(%), Delta_2:(hr)
		tp.readInputFile();
		tp.splitCustByTime();
		tp.calcDist();

		// initial solution
		// including savings algo. & find shortest path(if isTest = True, then it won't do this part)
		bool is_test = false;
		tp.getInitSolution(is_test);

		// use same courier number (NOT DONE YET)
		vector<int> peak_time({time_period-1,time_period-1,time_period-1});
		tp.useSameNumCourier(peak_time);

		// workload balance
		int FIRST_SHORT = 1, LAST_LONG = 1;
		int FIRST_SHORT_R = 3, LAST_LONG_R = 3;
		tp.balanceWorkload(FIRST_SHORT, LAST_LONG, FIRST_SHORT_R, LAST_LONG_R);

		// increase familiarity
		int LOW_BOUND = 4; // need to lower than lower bound
		tp.increaseFamiliarity(LOW_BOUND);

		// show final results
		tp.showFinalResult();
	}
	
	

	

	// main function's return value
	return 0;
}
