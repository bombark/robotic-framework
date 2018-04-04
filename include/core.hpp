/*============================================================================*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <map>

using namespace std;

typedef std::string String;

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

struct Coll;
struct Ctx;

struct Api {
	void   (*set_nat)(Ctx* ctx, uint64_t val);
	void   (*set_int)(Ctx* ctx, int64_t  val);
	void   (*set_flt)(Ctx* ctx, double   val);
	void   (*set_str)(Ctx* ctx, String   val);

	uint64_t (*to_nat)(Ctx* ctx);
	int64_t  (*to_int)(Ctx* ctx);
	double   (*to_flt)(Ctx* ctx);
	String   (*to_str)(Ctx* ctx);
	Ctx      (*to_coll)(Ctx* ctx);

	Stat   (*stat)(Ctx* ctx);
	void   (*end)(Ctx* ctx);

	void   (*seek_nat)(Ctx* ctx, uint64_t idx);
	void   (*seek_int)(Ctx* ctx, int64_t  idx);
	void   (*seek_str)(Ctx* ctx, String   idx);

	String (*serialize)(Coll* coll);

	Ctx    (*mkmap)(Ctx* ctx);
	Ctx    (*subopen)(Ctx* ctx);
};



struct Ctx {
	//Ctx*  ctx;
	Coll* coll;
	void* data;
	Api   api;

	size_t root_i;
	size_t path_i;
	size_t idx_i;
	string root_s;
	string path_s;
	string idx_s;

	Ctx(){
		this->root_i = 0;
		this->idx_i  = 0;
	}

	Ctx(const Ctx& ctx){
		this->coll   = ctx.coll;
		this->api    = ctx.api;
		this->data   = ctx.data;
		this->root_i = ctx.root_i;
		this->idx_i  = 0;
		this->root_s = ctx.root_s;
	}

	Ctx(Coll* ctx);

	inline Ctx& end(){this->api.end(this);return *this;}

	inline void seekNat(uint64_t val){
		this->path_i = val;
		this->api.seek_nat(this,val);
	}
	inline void seekInt(int64_t  val){this->api.seek_int(this,val);}
	inline void seekStr(String   val){this->api.seek_str(this,val);}


	inline void setNat(uint64_t val){this->api.set_nat(this,val);}
	inline void setInt(int64_t val){this->api.set_int(this,val);}
	inline void setFlt(double val){this->api.set_flt(this,val);}
	inline void setStr(String val){this->api.set_str(this,val);}


	inline Ctx  mkmap(){return this->api.mkmap(this);}



	inline uint64_t toNat(){return this->api.to_nat(this);}
	inline int64_t  toInt(){return this->api.to_int(this);}
	inline double   toFlt(){return this->api.to_flt(this);}
	inline String   toStr(){return this->api.to_str(this);}
	inline Ctx      toColl(){return this->api.to_coll(this);}


	inline bool isRootVet();
	inline bool isRootMap();


	inline bool isNull(){this->stat().isNull();}

	inline bool isItem(){this->stat().isItem();}
	inline bool isNat(){this->stat().isNat();}
	inline bool isInt(){this->stat().isInt();}
	inline bool isFlt(){this->stat().isFlt();}
	inline bool isStr(){this->stat().isStr();}

	inline bool isColl(){this->stat().isColl();}
	inline bool isVet(){this->stat().isVet();}
	inline bool isMap(){this->stat().isMap();}



	inline String serialize(){
		return this->api.serialize(this->coll);
	}

	inline Stat   stat(){this->api.stat(this);}
	inline size_t size(){return 0;}

	// Friend Function
	friend std::ostream& operator<<(std::ostream& out, Ctx& ctx){
		out << ctx.serialize();
		return out;
	}
};


struct CollPtr : Ctx {
	CollPtr(){}
	CollPtr(Coll* coll) : Ctx(coll){}
};


struct VetCtx1 : Ctx {
	inline VetCtx1& operator[](size_t val){this->seekNat(val); return *this;}
	inline VetCtx1& operator[](String val){this->seekStr(val); return *this;}

	inline size_t size(){return 0;}

	inline void operator=(uint8_t  val){this->setNat(val);}
	inline void operator=(uint16_t val){this->setNat(val);}
	inline void operator=(uint32_t val){this->setNat(val);}
	inline void operator=(uint64_t val){this->setNat(val);}
	inline void operator=(int8_t  val){this->setInt(val);}
	inline void operator=(int16_t val){this->setInt(val);}
	inline void operator=(int32_t val){this->setInt(val);}
	inline void operator=(int64_t val){this->setInt(val);}
	inline void operator=(float val){this->setFlt(val);}
	inline void operator=(double val){this->setFlt(val);}
	inline void operator=(String val){this->setStr(val);}
};


struct VetPtr : CollPtr {
	VetPtr(){}
	VetPtr(const Ctx& ctx) {
		cout << "opa\n";
	}
	VetPtr(const VetCtx1& ctx) {
		cout << "aqui\n";
	}

	VetPtr(Coll* coll) : CollPtr(coll){
		cout << "eee\n";
	}


	inline void prev(){this->seekInt(-1);}
	inline void next(){this->seekInt(+1);}

	inline VetPtr& operator>>(uint64_t& val){
		val = this->toNat();
		this->next();
		return *this;
	}
	inline VetPtr& operator>>(int64_t&  val){
		val = this->toInt();
		this->next();
		return *this;
	}
	inline VetPtr& operator>>(double&   val){
		val = this->toFlt();
		this->next();
		return *this;
	}
	inline VetPtr& operator>>(String&   val){
		val = this->toStr();
		this->next();
		return *this;
	}

	inline VetCtx1& operator[](size_t val){
		this->seekNat(val);
		return *((VetCtx1*) this);
	}
	inline VetCtx1& operator[](String val){
		this->seekStr(val);
		return *((VetCtx1*) this);
	 }
};

struct VetIt : Ctx {
	inline VetIt& operator++(){
		this->seekInt(+1); return *this;
	}

	inline VetIt& operator--(){
		this->seekInt(+1); return *this;
	}
};


struct MapCtx1 : Ctx {
	inline MapCtx1& operator[](String idx){
cout << "1:" << idx << endl;
		//this->seekStr(idx);
		return *this;
	}

	inline Stat   stat(){return Stat(0);}
	inline size_t size(){return 0;}

	inline void operator=(uint8_t  val){this->setNat(val);}
	inline void operator=(uint16_t val){this->setNat(val);}
	inline void operator=(uint32_t val){this->setNat(val);}
	inline void operator=(uint64_t val){this->setNat(val);}
	inline void operator=(int8_t  val){this->setInt(val);}
	inline void operator=(int16_t val){this->setInt(val);}
	inline void operator=(int32_t val){this->setInt(val);}
	inline void operator=(int64_t val){this->setInt(val);}
	inline void operator=(float val){this->setFlt(val);}
	inline void operator=(double val){this->setFlt(val);}
	inline void operator=(String val){this->setStr(val);}
};

struct MapCtx0 : Ctx {
	MapCtx0(){}

	MapCtx0(const Ctx& ctx) : Ctx(ctx){

	}

	inline MapCtx1& operator[](String idx){
		//this->seekStr(idx);
cout << "0:" << idx << endl;
		return *((MapCtx1*)this);
	}
};

typedef MapCtx0 MapPtr;




struct Cursor {
	Ctx ctx;

	Cursor(){}
	Cursor(const Ctx ctx){
		this->ctx = ctx;
	}

	inline void operator++(){}//this->ctx.next();}

	inline bool isOk(){return this->ctx.stat().isPresent();}
	inline Ctx  row(){return this->ctx.toColl();}
};


struct CtxPtr : Ctx {
	Ctx base;
	String root_s;
	size_t root_i;


};


struct MapIt : Ctx {
	Ctx base;

	MapIt(const Ctx& ctx) : Ctx(ctx){

	}

	MapIt(const Cursor cur){}
	inline bool isOk(){this->stat().isPresent();}
	inline void operator++(){this->seekInt(+1);}

	inline String id(){return this->idx_s;}

	static String ctx_to_str(Ctx* ctx){
		//MapIt* it = (MapIt*) ctx;
		//return it->base[ it->idx_s ].toStr();
	}

	static void ctx_set_str(Ctx* ctx, String val){
		//MapIt* it = (MapIt*) ctx;
		//it->base[ it->idx_s ].setStr(val);
	}


};

/*----------------------------------------------------------------------------*/




/*============================================================================*/

struct Coll  {
	Stat stat;
	Ctx  ctx;

	Coll(){
		this->ctx.coll = this;
		this->ctx.api.to_str   = ctx_to_str;
		this->ctx.api.end      = ctx_end;
		this->ctx.api.serialize = ctx_serialize;
		this->ctx.api.stat     = ctx_stat;
		this->ctx.api.seek_nat = ctx_seek_nat;
		this->ctx.api.seek_int = ctx_seek_int;
		this->ctx.api.seek_str = ctx_seek_str;
	}

	virtual void buildCtx(Ctx& ctx){
		ctx = this->ctx;
	}

	virtual Cursor begin(){}

	virtual size_t size()=0;

	virtual void putNat(uint64_t val)=0;
	virtual void putInt(int64_t  val)=0;
	virtual void putFlt(double   val)=0;
	virtual void putStr(String   val)=0;
	virtual void putColl(Coll* coll)=0;

	// Macros
	/*inline uint64_t toNat(){return ctx.toNat();}
	inline int64_t  toInt(){return ctx.toInt();}
	inline double   toFlt(){return ctx.toFlt();}*/
	inline String   toStr(){
		//return ctx.toStr();
		return this->ctx.serialize();
	}

	// Contexts Functions
	static void   ctx_put_str(Ctx* ctx, String val){}
	static String ctx_to_str(Ctx* ctx){return "";}
	static void   ctx_end(Ctx* ctx){}
	static Stat   ctx_stat(Ctx* ctx){return Stat(0);}

	static void   ctx_seek_nat(Ctx* ctx, uint64_t idx){
		ctx->idx_i  = 0;
	}

	static void   ctx_seek_int(Ctx* ctx, int64_t idx){
		ctx->idx_i   = 0;
	}

	static void   ctx_seek_str(Ctx* ctx, String   idx){
		ctx->idx_s.clear();
	}

	static String ctx_serialize(Coll* coll){
		String res;
		if ( coll->stat.isVet() ){
			VetPtr vetptr(coll);
			res += '[';
			size_t size = vetptr.size();
			if ( size > 0 ){
				res += vetptr[0].toStr();
				for (size_t i=1; i<size; i++){
					res += ',';
					//if ( vetptr[i].isColl() ){
						//res += this->ctx_serialize(  );
					//} else {
						res += vetptr[i].toStr();
					//}
				}
			}
			res += ']';
		} else if ( coll->stat.isMap() ){
			res += "{\n";
			for ( Cursor it=coll->begin(); it.isOk(); ++it ){
				VetPtr row = it.row();
				res += '\t';
				res += row[0].toStr();
				res += ": ";
				res += row[1].toStr();
				res += '\n';
			}
			res += '}';
		}
		return res;
	}

	// Friend Function
	friend std::ostream& operator<<(std::ostream& out, Coll& coll){
		out << coll.ctx.serialize();
		return out;
	}
};

/*----------------------------------------------------------------------------*/




/*============================================================================*/

inline Ctx::Ctx(Coll* coll){
	this->coll = coll;
	coll->buildCtx(*this);
}

inline bool Ctx::isRootVet(){this->coll->stat.isVet();}
inline bool Ctx::isRootMap(){this->coll->stat.isMap();}

/*inline Ctx& Ctx::putNat(uint64_t val){this->coll->putNat(val);return *this;}
inline Ctx& Ctx::putInt(int64_t  val){this->coll->putInt(val);return *this;}
inline Ctx& Ctx::putFlt(double   val){this->coll->putFlt(val);return *this;}
inline Ctx& Ctx::putStr(String   val){this->coll->putStr(val);return *this;}*/

//inline size_t Ctx::size(){return this->coll->size();}

/*----------------------------------------------------------------------------*/




/*============================================================================*/


struct Vet : Coll {

	Vet(){
		stat.setVet();
		this->ctx.api.seek_nat = ctx_seek_nat;
		this->ctx.api.seek_int = ctx_seek_int;
		this->ctx.api.seek_str = ctx_seek_str;
	}

	inline VetCtx1& operator[](size_t val){
		this->ctx.seekNat(val);
		return (VetCtx1&) this->ctx;
	}

	inline Vet& operator<<(uint8_t  val){this->putInt(val); return *this;}
	inline Vet& operator<<(uint16_t val){this->putInt(val); return *this;}
	inline Vet& operator<<(uint32_t val){this->putInt(val); return *this;}
	inline Vet& operator<<(uint64_t val){this->putInt(val); return *this;}
	inline Vet& operator<<(int8_t   val){this->putInt(val); return *this;}
	inline Vet& operator<<(int16_t  val){this->putInt(val); return *this;}
	inline Vet& operator<<(int32_t  val){this->putInt(val); return *this;}
	inline Vet& operator<<(int64_t  val){this->putInt(val); return *this;}
	inline Vet& operator<<(float    val){this->putFlt(val); return *this;}
	inline Vet& operator<<(double   val){this->putFlt(val); return *this;}
	inline Vet& operator<<(String   val){this->putStr(val); return *this;}
	inline Vet& operator<<(Coll&    val){this->putColl(&val); return *this;}


	static void   ctx_seek_nat(Ctx* ctx, uint64_t idx){
		ctx->path_i = idx;
		ctx->idx_i  = ctx->root_i + idx;
	}

	static void   ctx_seek_int(Ctx* ctx, int64_t idx){
		ctx->path_i += idx;
		ctx->idx_i   = ctx->root_i + ctx->path_i;
	}

	static void   ctx_seek_str(Ctx* ctx, String   idx){
		ctx->idx_s = idx;
	}

};

struct VetFixed : Vet {
	void putNat(uint64_t val){}
	void putInt(int64_t  val){}
	void putFlt(double   val){}
	void putStr(String   val){}
	void putColl(Coll* coll){}
};


struct Map : Coll {
	Map(){
		stat.setMap();
	}

	void putNat(uint64_t val){}
	void putInt(int64_t  val){}
	void putFlt(double   val){}
	void putStr(String   val){}
	void putColl(Coll* coll){}

	inline MapCtx1& operator[](String val){
		this->ctx.seekStr(val);
		return (MapCtx1&) this->ctx;
	}
};

/*----------------------------------------------------------------------------*/
