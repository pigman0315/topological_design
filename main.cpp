#include "main.h" // class ReadFile / TopoSolution1

//
// Global variables
//
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
string DIR_PATH = "./test_case/test1/";
const float H = 2.0;
float T; // unit: hr
const float SPEED = 40000; // unit: km/hr
const float SERV_COST = 0.0167; // 1.5 min = 0.025 hr
const int time_period = 3;
const int MAX_POSTAL_NUM = 6;

//
// Main function
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
	// Get result of each process
	//
	TopoSolution tp(district_customers_1st,exch_points_1st,0.03,3);
	tp.readInputFile();
	tp.splitCustByTime();
	tp.calcDist();

	// initial solution
	// including savings algo. & find shortest path(if isTest = True, then it won't do this part)
	bool isTest = true;
	tp.getInitSolution(isTest);

	// use same courier number 
	vector<int> courier_limit({2,2,2});
	tp.useSameNumCourier(courier_limit);

	// workload balance
	int FIRST_SHORT = 1, LAST_LONG = 1;
	tp.balanceWorkload(FIRST_SHORT, LAST_LONG);

	// increase familiarity
	tp.increaseFamiliarity(3);


	// main function's return value
	return 0;
}