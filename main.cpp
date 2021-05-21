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
const float H = 2.0;
float T; // unit: hr
const float SPEED = 40000; // unit: km/hr
const float SERV_COST = 0.0; // 1.5 min = 0.025 hr
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
	// Find result
	//
	TopoSolution tp(district_customers_1st,exch_points_1st,1.0,1.0);
	tp.readInputFile();
	tp.splitCustByTime();
	tp.calcDist();

	//
	bool isTest = false;
	tp.getInitSolution(isTest);

	//
	vector<int> courier_limit({2,3,2});
	tp.useSameNumCourier(courier_limit);

	//
	int FIRST_SHORT = 1, LAST_LONG = 1;
	tp.balanceWorkload(FIRST_SHORT, LAST_LONG);

	// 
	tp.increaseFamiliarity(3);


	// main function's return value
	return 0;
}