/*============================================================================*/

#include <vector>
#include <map>

/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct ItemStr : ItemApi {
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


	void     clear(){
		this->data.clear();
		this->type.setItem();
	}

	uint64_t toNat  (){return 0;}
	int64_t  toInt  (){return 0;}
	double   toFlt  (){return 0;}
	void     putNat (uint64_t val){}
	void     putInt (uint64_t val){}
	void     putFlt (double   val){}
	void     read_bytes (void* dst, size_t ini, size_t size){}
	void     write_bytes(void* src, size_t ini, size_t size){}

	size_t   row_get_id(){}
	void     row_next(){}
	void     row_prev(){}


	Stat     node_stat  (Node& node){return Stat(0);}
	uint64_t node_size  (Node& node){return 1;}
	//String   node_to_str(Node& node){return "";}//data[node.idx_i];}

	void     node_put_str(Node& node, String val){}
	void     node_make(Node& node, Stat type){}

	void     node_join_nat(Node& out, uint64_t idx, Node* base){}
	void     node_join_str(Node&, String, Node*){}
};

/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct VetUnit : CollApi {
	vector<Unit*> data;

	Stat     stat(){return Stat().setVet();}
	uint64_t size(){return data.size();}
	void     clear(){data.clear();}

	void     putNat (uint64_t val){
		ItemStr* item = new ItemStr(val);
		data.push_back(item);
	}

	void     putInt (uint64_t val){
		ItemStr* item = new ItemStr(val);
		data.push_back(item);
	}

	void     putFlt (double   val){
		ItemStr* item = new ItemStr(val);
		data.push_back(item);
	}

	void     putStr (String   val){
		ItemStr* item = new ItemStr(val);
		data.push_back(item);
	}

	Node     begin(){}
	size_t   row_get_id(){}
	void     row_next(){}
	void     row_prev(){}

	Stat     node_stat (Node& node){
		if ( node.idx_i >= data.size() )
			return Stat(0);
		return data[node.idx_i]->stat();
	}

	uint64_t node_size (Node& node){
		return data[node.idx_i]->size();
	}

	void     node_clear(Node& node, bool erase_idx=false, bool recursive=false){
		return data[node.idx_i]->clear();
	}

	void     node_make (Node& node, Stat type){

	}

	uint64_t node_to_nat (Node& node, uint64_t notdef){
		Unit* unit = data[node.idx_i];
		return  unit->isItem() ? unit->toNat() : notdef;
	}

	int64_t  node_to_int (Node& node, int64_t  notdef){
		Unit* unit = data[node.idx_i];
		return  unit->isItem() ? unit->toInt() : notdef;
	}

	double   node_to_flt (Node& node, double   notdef){
		Unit* unit = data[node.idx_i];
		return  unit->isItem() ? unit->toFlt() : notdef;
	}

	String   node_to_str (Node& node, const char* notdef){
		Unit* unit = data[node.idx_i];
		return  unit->isItem() ? unit->toStr() : notdef;
	}

	void     node_put_nat (Node& node, uint64_t val){

	}

	void     node_put_int (Node& node, uint64_t val){

	}

	void     node_put_flt (Node& node, double   val){

	}

	void     node_put_str (Node& node, String   val){

	}

	Node     node_open(Node& node){

	}

	void     node_join_nat(Node& out, uint64_t idx, Node* base){}
	void     node_join_str(Node& out, String   idx, Node* base){}
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct MapUnit : CollApi {
	std::map<String,Unit*> data;

	Stat     stat(){return Stat().setMap();}
	uint64_t size(){return this->data.size();}

	void     putStr(String val){}


	struct iterator : CollApi {
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


		void     clear(){}
		void     putNat (uint64_t val){}
		void     putInt (uint64_t val){}
		void     putFlt (double   val){}
		Node     begin(){return Node(NULL);}
		size_t   row_get_id(){}
		void     row_next(){++this->it;}
		void     row_prev(){--this->it;}


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

		void     node_clear(Node& node, bool erase_idx=false, bool recursive=false){}
		uint64_t node_to_nat (Node& node, uint64_t notdef){return notdef;}
		int64_t  node_to_int (Node& node, int64_t  notdef){return notdef;}
		double   node_to_flt (Node& node, double   notdef){return notdef;}
		String   node_to_str (Node& node, const char* notdef){
			if ( node.idx_i == 0 )
				return it->first;
			else if ( node.idx_i == 1 )
				return it->second->toStr();
			else
				return notdef;
		}

		void     node_put_nat (Node& node, uint64_t val){}
		void     node_put_int (Node& node, uint64_t val){}
		void     node_put_flt (Node& node, double   val){}
		Node     node_open(Node& node){}


		void     node_join_nat(Node& out, uint64_t idx, Node* base){}
		void     node_join_str(Node& out, String   idx, Node* base){}
		void     node_put_str(Node& node, String val){}
		void     node_make(Node& node, Stat type){}
	};

	void     clear(){

	}


	void     putNat (uint64_t val){}
	void     putInt (uint64_t val){}
	void     putFlt (double   val){}
	size_t   row_get_id(){}
	void     row_next(){}
	void     row_prev(){}

	Stat     node_stat  (Node& node){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			return it->second->stat();
		}
		return Stat(0);
	}

	void     node_clear(Node& node, bool erase_idx=false, bool recursive=false){}

	uint64_t node_size(Node& node){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			return it->second->size();
		}
		return 0;
	}


	uint64_t node_to_nat (Node& node, uint64_t notdef){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() && it->second->isItem() ){
			return it->second->toNat();
		}
		return notdef;
	}

	int64_t  node_to_int (Node& node, int64_t  notdef){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() && it->second->isItem() ){
			return it->second->toInt();
		}
		return notdef;
	}

	double   node_to_flt (Node& node, double   notdef){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() && it->second->isItem() ){
			return it->second->toFlt();
		}
		return notdef;
	}

	String   node_to_str (Node& node, const char* notdef){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			return it->second->toStr();
		}
		return notdef;
	}

	void node_put_nat (Node& node, uint64_t val){}
	void node_put_int (Node& node, uint64_t val){}
	void node_put_flt (Node& node, double   val){}
	void node_put_str(Node& node, String val){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			//it->second->putStr(val);
		} else {
			ItemStr* item = new ItemStr(val);
			data.insert( std::pair<String,Unit*>(node.idx_s,item) );
		}
	}

	void node_put_unit(Node& node, Unit* unit, bool reference=false){
		std::map<String,Unit*>::iterator it = data.find(node.idx_s);
		if ( it != data.end() ){
			//it->second->putStr(val);
		} else {
			data.insert( std::pair<String,Unit*>(node.idx_s,unit) );
		}
	}


	Node     begin(){
		Node res;
		res.unit  = new MapUnit::iterator(*this);
		res.idx_i = 0;
		return res;
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
			} else if ( type.isVet() ){
				VetUnit* vet = new VetUnit();
				data.insert( std::pair<String,Unit*>(node.idx_s,vet) );
			} else if ( type.isMap() ){
				MapUnit* map = new MapUnit();
				data.insert( std::pair<String,Unit*>(node.idx_s,map) );
			}
		}
	}
};

/*----------------------------------------------------------------------------*/
