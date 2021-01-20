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
float T = 0.29; //hr
float SPEED = 40000; //km/hr
//
int main(){
	// read file
	ReadFile rf;
	rf.run();

	// do randomized savings algo.
	srand(time(NULL)); // random seed
	SavingsAlgo sa(district_customers_1st[0],exch_points_1st[0]);
	sa.run();
}