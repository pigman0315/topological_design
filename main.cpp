#include "main.h"
// Global variables
int w, m_I, m_O, best_rot_deg;
Node best_1st_center;
vector<Node> best_2nd_center;
vector< vector<Node> > best_3rd_center;
vector< vector<Node> > district_endpoints_1st;
vector< vector< vector<Node> > > district_endpoints_2nd;
vector< vector<Node> > district_customers_1st;
vector< vector< vector<Node> > > district_customers_2nd;
//
int main(){
	// read file
	ReadFile rf;
	rf.run();
}