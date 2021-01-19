#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include "sp2.h"
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
class ReadFile{
public:
	void run(){
		ifstream file;
		file.open("sp1_result.txt");
		string str;
		// get w, get m_I, m_O, best rotation degree
		getline(file,str);
		w = stoi(str);
		getline(file,str);
		m_I = stoi(str);
		getline(file,str);
		m_O = stoi(str);
		getline(file,str);
		best_rot_deg = stoi(str);
		// get best 1st layer center
		getline(file,str);
		best_1st_center = get_node(str);
		// get best 2nd layer center
		for(int i = 0;i < m_I;i++){
			getline(file,str);
			Node tmp = get_node(str);
			best_2nd_center.push_back(tmp);
		}
		// get 1st district endpoints
		for(int i = 0;i < m_I;i++){
			getline(file,str);
			int num = stoi(str);
			vector<Node> tmp_vec;
			for(int j = 0;j < num;j++){
				getline(file,str);
				Node tmp_n = get_node(str);
				tmp_vec.push_back(tmp_n);
			}
			district_endpoints_1st.push_back(tmp_vec);
		}
		if(w == 1){
			// get 1st layer districted customer points
			for(int i = 0;i < m_I;i++){
				getline(file,str);
				int num = stoi(str);
				vector<Node> tmp_vec;
				for(int j = 0;j < num;j++){
					getline(file,str);
					Node tmp_n = get_node(str);
					tmp_vec.push_back(tmp_n);
				}
				district_customers_1st.push_back(tmp_vec);
			}
			// get 1st layer exchange points
			for(int i = 0;i < m_I;i++){
				getline(file,str);
				Node tmp = get_node(str);
				exch_points_1st.push_back(tmp);
			}
		}
		if(w == 2){
			// get best 3rd layer center
			for(int i = 0;i < m_I;i++){
				vector<Node> tmp_v;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					Node tmp = get_node(str);
					tmp_v.push_back(tmp);
				}
				best_3rd_center.push_back(tmp_v);
			}
			// get 2nd district endpoints
			for(int i = 0;i < m_I;i++){
				vector< vector<Node> > tmp_vv;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					int num = stoi(str);
					vector<Node> tmp_v;
					for(int k = 0;k < num;k++){
						getline(file,str);
						Node tmp_n = get_node(str);
						tmp_v.push_back(tmp_n);
					}
					tmp_vv.push_back(tmp_v);
				}
				district_endpoints_2nd.push_back(tmp_vv);
			}
			// get 2nd districted customer points
			for(int i = 0;i < m_I;i++){
				vector< vector<Node> > tmp_vv;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					int num = stoi(str);
					vector<Node> tmp_v;
					for(int k = 0;k < num;k++){
						getline(file,str);
						Node tmp_n = get_node(str);
						tmp_v.push_back(tmp_n);
					}
					tmp_vv.push_back(tmp_v);
				}
				district_customers_2nd.push_back(tmp_vv);
			}
			// get 1st layer exchange points
			for(int i = 0;i < m_I;i++){
				getline(file,str);
				Node tmp = get_node(str);
				exch_points_1st.push_back(tmp);
			}
			// get 1st layer exchange points
			// get best 3rd layer center
			for(int i = 0;i < m_I;i++){
				vector<Node> tmp_v;
				for(int j = 0;j < m_O+1;j++){
					getline(file,str);
					Node tmp = get_node(str);
					tmp_v.push_back(tmp);
				}
				exch_points_2nd.push_back(tmp_v);
			}
		}
		file.close();
	}
	Node get_node(string str){
		int idx;
		Node tmp;
		idx = str.find(" ");
		tmp.x = stoi(str.substr(0,idx));
		tmp.y = stoi(str.substr(idx+1,str.size()-idx));
		//tmp.show();
		return tmp;
	}
};

