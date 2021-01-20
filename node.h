#include <iostream>
using namespace std;
class Node{
public:
	float x;
	float y;
	Node(){
		x = -1;
		y = -1;
	}
	Node(int nx, int ny){
		x = nx;
		y = ny;
	}
	bool operator== (const Node& n) const{
		return (x == n.x && y == n.y);
	}
	bool operator< (const Node& n) const{
		// use Euclidean distance to origin point to determine which node is bigger
    	return (x*x+y*y) < (n.x*n.x+n.y*n.y);
	}
	void show(){
		cout << x << " " << y << endl;
	}
};
class SavingsNode{
public:
	float value;
	int i,j;
	Node ni;
	Node nj;
	SavingsNode(){
		value = -1;
	}
	SavingsNode(float v, int i_, int j_, Node n1, Node n2){
		value = v;
		i = i_;
		j = j_;
		ni = n1;
		nj = n2;
	}
	void show(){
		cout << "Saving value: " << value << endl;
		cout << "i: " << i << ", j: " << j << endl;
		ni.show();
		nj.show(); 
	}
};