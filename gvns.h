#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <random>
#include <chrono>

#ifndef _NODE_
#define _NODE_
#include "node.h" // class Node/SavingsNode/SolutionNode
#endif
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
extern float T;
extern const float SPEED;
//
class GVNS{
public:
	// variables
	SolutionNode solution;
	vector<Node> customer_points;
	Node exch_point;
	int customer_num;
public:
	// functions
	GVNS(SolutionNode sn, vector<Node> cps, Node ep);
	void initial();
	void run();
	void show_result();
};

GVNS::GVNS(SolutionNode sn, vector<Node> cps, Node ep){
	exch_point = ep;
	customer_points = cps;
	solution = sn;
	customer_num = customer_points.size();
}
void GVNS::run(){
	solution.show();
}