/*============================================================================*/

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

/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct Stat {
	union {
		char   type[2];
		short  type16;
	};

	Stat(){this->type16=0;}
	Stat(int v){this->type16=v;}

	Stat& setNull(){type16 = 0;return *this;}
	Stat& setVet(){type[0]='c';type[1]='v';return *this;}
	Stat& setMap(){type[0]='c';type[1]='m';return *this;}

	Stat& setItem(){type[0]='i';return *this;}
	Stat& setNat(){setItem();type[1]='n';return *this;}
	Stat& setInt(){setItem();type[1]='i';return *this;}
	Stat& setFlt(){setItem();type[1]='f';return *this;}
	Stat& setStr(){setItem();type[1]='s';return *this;}

	bool isNull(){return type16==0;}
	bool isPresent(){return type16!=0;}

	bool isColl(){return type[0]=='c';}
	bool isVet(){return isColl() && type[1]=='v';}
	bool isMap(){return isColl() && type[1]=='m';}

	bool isItem(){return type[0]=='i';}
	bool isNat(){return isItem() && type[1]=='n';}
	bool isInt(){return isItem() && type[1]=='i';}
	bool isFlt(){return isItem() && type[1]=='f';}
	bool isStr(){return isItem() && type[1]=='s';}

	// Friend Function
	friend std::ostream& operator<<(std::ostream& out, Stat stat){
		out << stat.type[0] << stat.type[1];
		return out;
	}
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/

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

	inline Stat   stat();
	inline size_t size();
	inline Node   open();

	inline bool   isNull(){this->stat().isNull();}
	inline bool   isItem(){this->stat().isItem();}
	inline bool   isColl(){this->stat().isColl();}

	inline String toStr();

	inline Node   mkmap();

	inline void   set(String val);


	inline Node operator[](int val);
	inline Node operator[](String val);

	/*friend std::ostream& operator<<(std::ostream& out, Node& node){
		if ( node.isItem() ){
			out << "Item";
		} else if ( node.isColl() ){
			out << "Coll";
		}
		return out;
	}*/

	friend std::ostream& operator<<(std::ostream& out, Node node){
		if ( node.isNull() ){
			out << "Null";
		} else if ( node.isItem() ){
			out << "Item";
			if ( node.stat().isStr() ){
				out << "/Str:" << node.toStr();
			}
		} else if ( node.isColl() ){
			out << "Coll";
			if ( node.stat().isVet() ){
				out << "/Vet";
			} else if ( node.stat().isMap() ){
				out << "/Map";
			}
		}
		return out;
	}
};


struct Unit {
	// Unit - Basic
	virtual Stat     stat()=0;
	virtual uint64_t size()=0;
	virtual void     clear()=0;

	// Unit - Item
	virtual uint64_t toNat  ()=0;
	virtual int64_t  toInt  ()=0;
	virtual double   toFlt  ()=0;
	virtual String   toStr  ()=0;
	virtual void     putNat (uint64_t val)=0;
	virtual void     putInt (uint64_t val)=0;
	virtual void     putFlt (double   val)=0;
	virtual void     putStr (String   val)=0;

	virtual Node     open_item(bool only_read=false){} // end2begin, seeklast

	// Unit - Map e Vector
	virtual Node     begin(){}
	//virtual Node     select(String fields){}

	// Unit - Stream
	virtual size_t   row_get_id(){}
	virtual void     row_next(){}
	virtual void     row_prev(){}

	// Collection
	virtual Stat     node_stat (Node& node)=0;
	virtual uint64_t node_size (Node& node)=0;
	virtual void     node_clear(Node& node, bool erase_idx=false, bool recursive=false){}
	virtual void     node_make (Node& node, Stat type)=0;

	// Collection, where Node is a Item
	virtual uint64_t node_to_nat (Node& node, uint64_t notdef)=0;
	virtual int64_t  node_to_int (Node& node, int64_t  notdef)=0;
	virtual double   node_to_flt (Node& node, double   notdef)=0;
	virtual String   node_to_str (Node& node, String   notdef)=0;
	virtual void     node_put_nat (Node& node, uint64_t val)=0;
	virtual void     node_put_int (Node& node, uint64_t val)=0;
	virtual void     node_put_flt (Node& node, double   val)=0;
	virtual void     node_put_str (Node& node, String   val)=0;


	virtual Node     node_open(Node& node){}



	// com subnodes
	virtual void     node_join_nat(Node& out, uint64_t idx, Node* base)=0;
	virtual void     node_join_str(Node& out, String   idx, Node* base)=0;


	Node atNat(uint64_t idx){return Node(this, idx);}
	Node atStr(String   idx){return Node(this, idx);}
	Node operator[](size_t idx){return this->atNat(idx);}
	Node operator[](String idx){return this->atStr(idx);}

	inline bool isNull(){return this->stat().isNull();}
	inline bool isItem(){return this->stat().isItem();}
	inline bool isColl(){return this->stat().isColl();}
	inline bool isVet(){return this->stat().isVet();}
	inline bool isMap(){return this->stat().isMap();}


	friend std::ostream& operator<<(std::ostream& out, Unit& unit){
		if ( unit.isItem() ){
			out << "Item";
			if ( unit.stat().isInt() ){
				out << "/Int:" << unit.toStr();
			} else if ( unit.stat().isStr() ){
				out << "/Str:" << unit.toStr();
			}
		} else if ( unit.isColl() ){
			out << "Coll";
			if ( unit.stat().isVet() ){
				out << "/Vet:[";
				for (size_t i=0; i<unit.size(); i++){
					out << unit[i] << ";";
				}
				out << "]";
			} else if ( unit.stat().isMap() ){
				out << "/Map";
				/*for (MapIt it=map.begin(); it.isOk(); it.next() ){
					cout << it[0].toStr() << ": " << it[1].toStr() << endl;
				}*/
			}
		}
		return out;
	}
};


inline Stat   Node::stat(){
	return ( unit == NULL ) ? Stat(0) : unit->node_stat(*this);
}

inline size_t Node::size(){
	return this->unit->node_size(*this);
}

inline Node   Node::open(){
	return this->unit->node_open(*this);
}

inline String Node::toStr(){return this->unit->node_to_str(*this);}


inline void   Node::set(String val){this->unit->node_put_str(*this,val);}

inline Node   Node::mkmap(){
	this->unit->node_make(*this,Stat().setMap());
	return *this;
}

inline Node Node::operator[](int val){
	Node res;
	res.unit  = this->unit;
	res.level = this->level+1;
	this->unit->node_join_nat(res, val, this);
	return res;
}

inline Node Node::operator[](String val){
	Node res;
	res.unit  = this->unit;
	res.level = this->level+1;
	this->unit->node_join_str(res, val, this);
	return res;
}

/*----------------------------------------------------------------------------*/



struct MapIt : Node {
	MapIt(const Node node){
		this->unit  = node.unit;
		this->level = 0;
		this->idx_i = 0;
	}
	inline bool isOk(){return !this->unit->stat().isNull();}
	inline void next(){this->unit->row_next();}

	inline Node operator[](int val){
		Node res;
		res.unit  = this->unit;
		res.idx_i = val;
		return res;
	}
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


/*============================================================================*/

struct ItemStr : Unit {
	Stat type;
	union {
		uint64_t nat64;
		int64_t  int64;
		double   flt64;
	};
	string data;

	ItemStr(){}
	ItemStr(int val){
		type.setInt();
		this->int64 = val;
	}
	ItemStr(String val){
		type.setStr();
		this->data = val;
	}

	Stat     stat(){return type;}

	uint64_t size(){
		if ( type.isStr() ){
			return data.size();
		} else {
			return sizeof(int64_t);
		}
	}

	String   toStr(){
		if ( type.isStr() ){
			return this->data;
		} else {
			return "0";
		}
	}

	void     putStr(String val){data += val;}



	Stat     node_stat  (Node& node){return Stat(0);}
	uint64_t node_size  (Node& node){return 1;}
	String   node_to_str(Node& node){return "";}//data[node.idx_i];}

	void     node_put_str(Node& node, String val){}
	void     node_make(Node& node, Stat type){}

	void     node_join_nat(Node& out, uint64_t idx, Node* base){}
	void     node_join_str(Node&, String, Node*){}
};


struct VetStr : Unit {
	vector<String> data;

	Stat     stat(){
		return Stat().setVet();
	}
	uint64_t size(){return this->data.size();}

	void     putStr(String val){this->data.push_back(val);}


	Stat     node_stat  (Node& node){
		if ( node.idx_i >= this->data.size() )
			return Stat(0);
		return Stat().setStr();
	}

	void node_put_str(Node& node, String val){
		this->data[node.idx_i] = val;
	}

	void     node_join_nat(Node& out, uint64_t idx, Node* base){}
	void     node_join_str(Node& out, String   idx, Node* base){}
	void     node_make(Node& node, Stat type){}

	uint64_t node_size(Node& node){return 0;}
	String   node_to_str(Node& node){
		return this->data[node.idx_i];
	}

	void     node_join(Node& res, Node& base){
	}
};







struct MapUnit : Unit {
	std::map<String,Unit*> data;

	Stat     stat(){return Stat().setMap();}
	uint64_t size(){return this->data.size();}

	void     putStr(String val){}


	struct iterator : Unit {
		std::map<String,Unit*>::iterator it;
		std::map<String,Unit*>::iterator end;

		iterator(MapUnit& unit){
			this->it  = unit.data.begin();
			this->end = unit.data.end();
		}

		Stat     stat(){
			return  ( it != end ) ? Stat().setVet() : Stat(0);
		}

		uint64_t size(){return (it != end) ? 2 : 0;}

		void     putStr(String val){}

		void     nextRow(){
			++this->it;
		}

		Stat     node_stat  (Node& node){
			if ( node.idx_i == 0 ){
				return Stat().setStr();
			} else if ( node.idx_i == 1 ){
				return it->second->stat();
			}
			return Stat(0);
		}

		uint64_t node_size  (Node& node){
			if ( node.idx_i == 0 ){
				return it->first.size();
			} else if ( node.idx_i == 1 ){
				return it->second->size();
			}
			return 0;
		}

		String   node_to_str(Node& node){
			if ( node.idx_i == 0 )
				return it->first;
			else if ( node.idx_i == 1 )
				return it->second->toStr();
			else
				return "";
		}

		void     node_join_nat(Node& out, uint64_t idx, Node* base){}
		void     node_join_str(Node& out, String   idx, Node* base){}
		void     node_put_str(Node& node, String val){}
		void     node_make(Node& node, Stat type){}
	};

	Node     begin(){
		Node res;
		res.unit  = new MapUnit::iterator(*this);
		res.idx_i = 0;
		return res;
	}

	Stat     node_stat  (Node& node){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			return it->second->stat();
		}
		return Stat(0);
	}

	void node_put_str(Node& node, String val){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			//it->second->putStr(val);
		} else {
			ItemStr* item = new ItemStr(val);
			data.insert( std::pair<String,Unit*>(node.idx_s,item) );
		}
	}

	uint64_t node_size(Node& node){
		return 0;
	}

	String   node_to_str(Node& node){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			return it->second->toStr();
		}
		return "";
	}

	void     node_join_nat(Node& out, uint64_t idx, Node* base){
		out.idx_i = 0;
	}

	void     node_join_str(Node& out, String   idx, Node* base){
		out.idx_s = base->idx_s + "/" + idx;
	}

	Node     node_open(Node& node){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			return Node(it->second);
		} else {
			Node res;
			return res;
		}
	}

	void  node_make(Node& node, Stat type){
		if ( node.isNull() ){
			if ( type.isItem() ){
				ItemStr* item = new ItemStr("novo");
				data.insert( std::pair<String,Unit*>(node.idx_s,item) );
			} else if ( type.isMap() ){
				MapUnit* map = new MapUnit();
				data.insert( std::pair<String,Unit*>(node.idx_s,map) );
			}
		}
	}
};



int main(){
	MapUnit map;
	map["name"].set("opa1");
	map["score"].set("1000");
	map["friend"].set("joao");

	for (MapIt it=map.begin(); it.isOk(); it.next() ){
		cout << it[0].toStr() << ": " << it[1].toStr() << endl;
	}


	return 0;
}
