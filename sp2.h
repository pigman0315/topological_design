#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <math.h>
#include <algorithm>
#include "node.h"
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
//
struct SavingsNode{
	float value;
	int i,j;
	Node ni;
	Node nj;
	SavingsNode(float v, int i_, int j_, Node n1, Node n2){
		value = v;
		i = i_;
		j = j_;
		ni = n1;
		nj = n2;
	}
};
//
// randomized savings algo.
class SavingsAlgo{
public:
	// variables
	vector<SavingsNode> savings_list;
	vector<Node> customer_points;
	int customer_num;
	Node exch_point;
	// functions
	SavingsAlgo(vector<Node> cps, Node ep);
	void run();
	void get_savings();
	float get_dist(Node n1, Node n2);
};
// constructor
SavingsAlgo::SavingsAlgo(vector<Node> cps, Node ep){
	customer_points = cps;
	exch_point = ep;
	customer_num = customer_points.size();
}
// funciton
void SavingsAlgo::run(){
	// calculate savings value for each pair of node
	get_savings();

}
float SavingsAlgo::get_dist(Node n1, Node n2){
	return sqrt((n1.x-n2.x)*(n1.x-n2.x) + (n1.y-n2.y)*(n1.y-n2.y));
}
bool descend_func(SavingsNode sn1, SavingsNode sn2){
	return sn1.value > sn2.value;
}
void SavingsAlgo::get_savings(){
	// s_ij = d_i0 + d_0j - d_ij
	Node n0 = exch_point;
	for(int i = 0;i < customer_num;i++){
		for(int j = i+1;j < customer_num;j++){
			// prevent calculating savings value of same node
			if(i==j) continue;
			Node ni = customer_points[i];
			Node nj = customer_points[j];
			float tmp = get_dist(n0,ni) + get_dist(n0,nj) - get_dist(ni,nj);
			SavingsNode sn(tmp,i,j,ni,nj);
			savings_list.push_back(sn);
		}
	}
	// sort savings list descending
	sort(savings_list.begin(),savings_list.end(),descend_func);
	for(int i = 0;i < savings_list.size();i++){
		cout << savings_list[i].value << " " << savings_list[i].i << " " << savings_list[i].j <<endl;
		savings_list[i].ni.show();
		savings_list[i].nj.show();
	}
}


