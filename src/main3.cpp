//#include "core.hpp"
//#include "drv-basic.hpp"
//#include "drv-filesystem.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <map>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

typedef std::string String;

using namespace std;


struct Unit;


struct Node {
	Unit*   unit;
	int     level;
	int64_t idx_i;
	String  idx_s;

	Node(){}

	Node(Unit* unit, uint64_t idx=0){
		this->unit  = unit;
		this->level = 0;
		this->idx_i = idx;
	}

	Node(Unit* unit, String idx){
		this->unit  = unit;
		this->level = 0;
		this->idx_i = 0;
		this->idx_s = idx;
	}

	inline String toStr();


	inline void   set(String val);

	inline Node operator[](int val){
		Node res;
		res.unit  = this->unit;
		res.level = this->level+1;
		res.idx_i = this->idx_i + val;
		res.idx_s = this->idx_s;
		return res;
	}

	friend std::ostream& operator<<(std::ostream& out, const Node& node){
		out << node.unit << ": " << node.idx_i;
		return out;
	}
};


struct Unit {
	virtual uint64_t stat(){}
	virtual uint64_t size()=0;
	virtual void     putStr(String val)=0;


	virtual uint64_t node_stat  (Node& node){}
	virtual uint64_t node_size  (Node& node)=0;
	virtual String   node_to_str(Node& node)=0;

	virtual void     node_put_str(Node& node, String val)=0;

	Node atNat(uint64_t idx){return Node(this, idx);}
	Node atStr(String   idx){return Node(this, idx);}
	Node operator[](size_t idx){return this->atNat(idx);}
	Node operator[](String idx){return this->atStr(idx);}
};


inline String Node::toStr(){return this->unit->node_to_str(*this);}
inline void   Node::set(String val){this->unit->node_put_str(*this,val);}




struct ItemStr : Unit {
	string data;

	uint64_t size(){return data.size();}
	void     putStr(String val){data += val;}

	uint64_t node_stat  (Node& node){}
	uint64_t node_size  (Node& node){return 1;}
	String   node_to_str(Node& node){return data[node.idx_i];}

	void     node_put_str(Node& node, String val){}
};


struct VetStr : Unit {
	vector<String> data;

	uint64_t size(){return this->data.size();}

	void     putStr(String val){this->data.push_back(val);}

	void node_put_str(Node& node, String val){
		this->data[node.idx_i] = val;
	}

	uint64_t node_size(Node& node){return 0;}
	String   node_to_str(Node& node){
		return this->data[node.idx_i];
	}
};




int main(){
	VetStr vet;

	Fs fs;
	Node n = fs["./texto.txt"];

	Node file = n.open();
	file = "felipe bombardelli";

	PackPtr file;
	file("ddd").
}
