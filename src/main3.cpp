/*============================================================================*/

#include "core.hpp"
#include "drv-basic.hpp"
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


using namespace std;

/*----------------------------------------------------------------------------*/



struct FsFile : Unit {
	FILE* fd;
	string buf;
	struct stat buf_stat;

	FsFile(){
	}

	Stat     stat(){return Stat().setItem();}
	uint64_t size(){return 0;}
	void     clear(){}
	uint64_t toNat  (){}
	int64_t  toInt  (){}
	double   toFlt  (){}
	String   toStr  (){}
	void     putNat (uint64_t val){}
	void     putInt (uint64_t val){}
	void     putFlt (double   val){}
	void     putStr (String   val){}

	void     read_bytes (void* dst, size_t ini, size_t size){
		fseek(fd,ini,SEEK_SET);
		fread(dst,size,1,fd);
	}

	void     write_bytes(void* src, size_t ini, size_t size){
		fseek(fd,ini,SEEK_SET);
		fwrite(src,size,1,fd);
	}


	size_t   row_get_id(){}
	void     row_next(){}
	void     row_prev(){}


	virtual Stat     node_stat (Node& node)=0;
	virtual uint64_t node_size (Node& node)=0;
	virtual void     node_make (Node& node, Stat type)=0;
	virtual uint64_t node_to_nat (Node& node, uint64_t notdef)=0;
	virtual int64_t  node_to_int (Node& node, int64_t  notdef)=0;
	virtual double   node_to_flt (Node& node, double   notdef)=0;
	virtual String   node_to_str (Node& node, const char* notdef)=0;
	virtual void     node_put_nat (Node& node, uint64_t val)=0;
	virtual void     node_put_int (Node& node, uint64_t val)=0;
	virtual void     node_put_flt (Node& node, double   val)=0;
	virtual void     node_put_str (Node& node, String   val)=0;
	virtual void     node_join_nat(Node& out, uint64_t idx, Node* base)=0;
	virtual void     node_join_str(Node& out, String   idx, Node* base)=0;
};









/*============================================================================*/


struct UnitNode : Unit {
	Node base;

	UnitNode(){}

	UnitNode(const Node& base){
		this->base = base;
	}

	Stat     stat(){return base.stat();}

	uint64_t size(){return base.size();}

	void     putStr(String val){base.set(val);}

	Stat     node_stat  (Node& node){
		Node res;
		//base.unit->node_join(res,node,base);
		return res.stat();
	}

	uint64_t node_size  (Node& node){
		Node res;
		//base.unit->node_join(res,node,base);
		return res.size();
	}

	String   node_to_str(Node& node){
		Node res;
		//base.unit->node_join(res,node,base);
		return res.toStr();
	}

	void node_join_nat(Node& out, uint64_t idx, Node* base){
		out.idx_i = 0;
	}

	void node_join_str(Node& out, String idx, Node* base){
		out.idx_s = base->idx_s + "/" + idx;
	}

	void     node_put_str(Node& node, String val){
		Node res;
		res.unit = base.unit;
		base.unit->node_join_str(res,node.idx_s,&base);
		res.set(val);
	}

	void     node_make(Node& node, Stat type){
		Node res;
		//base.unit->node_join(res,node,base);
		//res.make();
	}
};



/*----------------------------------------------------------------------------*/






int main(){
	FsFile file;


	/*VetUnit vet;
	vet.putStr("opa1");
	vet.putStr("opa2");
	vet.putStr("opa3");
	cout << vet << endl;


	MapUnit p1;
	p1["name"].set("joao");




	for (MapIt it=p1.begin(); it.isOk(); it.next() ){
		cout << it[0].toStr() << ": " << it[1].toStr() << endl;
	}*/


	return 0;
}
