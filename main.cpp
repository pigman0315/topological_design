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
float T; //hr
const float SPEED = 40000; //km/hr
//
int main(){
	// read file
	ReadFile rf;
	rf.run();

	// show customer numbers
	cout << district_customers_1st[0].size() << endl;
	
	// calculate T by given formula
	T = H / (0.5+m_I+2*(w-1)*m_O);
	cout << "T: " << T << endl; 

	// do randomized savings algo.
	srand(time(NULL)); // random seed
	SavingsAlgo sa(district_customers_1st[0],exch_points_1st[0]);
	sa.run();

	//
	return 0;
}