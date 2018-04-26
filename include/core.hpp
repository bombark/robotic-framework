/*============================================================================*/

#include <stdio.h>
#include <stdint.h>
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
struct Stream;
struct Random;



struct Args {
	short base;
	short size;
	char  format[4];
	union {
		uint64_t nat64[4];
		int64_t  int64[4];
		double   flt64[4];
	};
	String str[4];

	Args(){this->clear();}

	void clear(){
		this->base = 0;
		this->size = 0;
	}


	void putNat(uint64_t val){
		nat64[size]  = val;
		format[size++] = 'u';
	}

	void putStr(String val){
		str[size] = val;
		format[size++] = 's';
	}

	uint64_t& atNat(int id){return nat64[base+id];}

	uint64_t getNat(int id){
		size_t idx = base + id;
		switch ( format[idx] ){
			case 'u': return nat64[idx];
			//case 's': return str[id];
			case 'd': return int64[idx];
			case 'f': return flt64[idx];
		}
		return 0;
	}

	String   getStr(int id){
		size_t idx = base + id;
		if ( format[idx] == 's' )
			return str[idx];
		else if ( format[idx] == 'u' ){
			char buf[64]; sprintf(buf,"%lu",nat64[idx]);
			return buf;
		} else if ( format[id] == 'd' ){
			char buf[64]; sprintf(buf,"%ld",int64[idx]);
			return buf;
		}  else if ( format[id] == 'f' ){
			char buf[64]; sprintf(buf,"%lf",flt64[idx]);
			return buf;
		}
		return "";
	}

	void operator=(uint64_t val){
		base = 0; size = 1;
		format[0] = 'u';
		nat64[0]  = val;
	}

	uint64_t nextNat(){
		uint64_t val = this->getNat(0); this->next();
		return val;
	}

	void next(){this->base += 1;}
};



struct Ctx {
	int    type;
	Unit*  unit;
	union {
		void*   data;
		char    d_char;
		int64_t d_int64;
	};
	Args   idx;

	Ctx(){type = 0;}
	Ctx(Unit* unit){this->unit = unit;}
	Ctx(Unit& unit){this->unit = &unit;}

	bool isNull(){return unit==NULL;}
	bool isStream(){return type==1;}
	bool isRandom(){return type==2;}
	bool isCtxRoot(){return type==3;}
	bool isCtxNode(){return type==4;}
};


struct Unit {
	void ric_resize(size_t size){}
	void ric_erase (size_t ini, size_t size, bool index_destroy){}

	void coll_erase(Ctx* ctx, bool index_destroy){}


	virtual ~Unit(){}

	virtual void openRandom (Random& pack)=0;
	virtual void openStream (Stream& pack)=0;

	virtual void get_raw(Ctx* ctx, void* dst, size_t size)=0;
	virtual void get_nat(Ctx* ctx, uint64_t& dst)=0;
	virtual void get_int(Ctx* ctx, int64_t&  dst)=0;
	virtual void get_flt(Ctx* ctx, double&   dst)=0;
	virtual void get_str(Ctx* ctx, String&   dst)=0;

	virtual void put_raw(Ctx* ctx, const void* src, size_t size)=0;
	virtual void put_nat(Ctx* ctx, uint64_t val)=0;
	virtual void put_int(Ctx* ctx, int64_t  val)=0;
	virtual void put_flt(Ctx* ctx, double   val)=0;
	virtual void put_str(Ctx* ctx, String   val)=0;
	virtual void put_cmd(Ctx* ctx, int type)=0;

	virtual uint64_t size()=0;
	//virtual void   seek(Ctx* ctx, size_t pos)=0;
	virtual String toStr()=0;
};


struct UnitBasic : Unit {
	void get_nat  (Ctx* ctx, uint64_t& dst){
		this->get_raw(ctx,(void*)&dst,sizeof(uint64_t));
	}

	void get_int  (Ctx* ctx, int64_t& dst){
		this->get_raw(ctx,(void*)&dst,sizeof(int64_t));
	}

	void get_flt  (Ctx* ctx, double& dst){
		this->get_raw(ctx,(void*)&dst,sizeof(double));
	}

	void put_nat  (Ctx* ctx, uint64_t val){
		this->put_raw(ctx,(void*)&val,sizeof(val));
	}

	void put_int  (Ctx* ctx, int64_t val){
		this->put_raw(ctx,(void*)&val,sizeof(val));
	}

	void put_flt  (Ctx* ctx, double val){
		this->put_raw(ctx,(void*)&val,sizeof(val));
	}

	void put_str(Ctx* ctx, String str){
		this->put_raw(ctx,str.c_str(),str.size());
	}
};


struct CtxData : Ctx {
	CtxData() : Ctx() {}
	CtxData(Unit* unit) : Ctx(unit){}
	CtxData(Unit& unit) : Ctx(unit){}

	uint64_t getNat(){
		uint64_t val; this->unit->get_nat(this,val);
		return val;
	}

	int64_t  getInt(){
		int64_t val; this->unit->get_int(this,val);
		return val;
	}

	double   getFlt(){
		double val; this->unit->get_flt(this,val);
		return val;
	}

	void getRaw(void* dst, size_t size){
		this->unit->get_raw(this,dst,size);
	}

	template <typename T> T getRawAs(){
		T res; this->unit->get_raw(this,&res,sizeof(T));
		return res;
	}

	void putRaw(const void* val, size_t size){this->unit->put_raw(this,val,size);}

	template <typename T> void putRawAs(T val){
		this->unit->put_raw(this,&val,sizeof(T));
	}

};



struct StreamNode;

struct Stream : CtxData {
	Stream() : CtxData() {}
	Stream(Unit* unit) : CtxData(unit){
		this->type=1;
		this->unit->openStream(*this);
	}
	Stream(Unit& unit) : CtxData(unit){
		this->type=1;
		this->unit->openStream(*this);
	}


	void putNat(uint64_t val){
		//this->unit->put_cmd(this,0);
		this->unit->put_nat(this,val);
		this->unit->put_cmd(this,1);
	}

	void putInt(int64_t  val){
		//this->unit->put_cmd(this,0);
		this->unit->put_int(this,val);
		this->unit->put_cmd(this,1);
	}

	void putFlt(double   val){
		//this->unit->put_cmd(this,0);
		this->unit->put_flt(this,val);
		this->unit->put_cmd(this,1);
	}

	void putStr(String val){
		//this->unit->put_cmd(this,0);
		this->unit->put_str(this,val);
		this->unit->put_cmd(this,1);
	}

	void endl(){this->unit->put_cmd(this,2);}

	Stream& put(uint8_t val){this->putNat(val);return *this;}
	Stream& put(uint16_t val){this->putNat(val);return *this;}
	Stream& put(uint32_t val){this->putNat(val);return *this;}
	Stream& put(uint64_t val){this->putNat(val);return *this;}

	Stream& put(int8_t val){this->putInt(val);return *this;}
	Stream& put(int16_t val){this->putInt(val);return *this;}
	Stream& put(int32_t val){this->putInt(val);return *this;}
	Stream& put(int64_t val){this->putInt(val);return *this;}

	Stream& put(float val){this->putFlt(val);return *this;}
	Stream& put(double val){this->putFlt(val);return *this;}

	Stream& put(String val){this->putStr(val);return *this;}
	Stream& put(void* val, size_t size){this->putRaw(val,size);return *this;}

	size_t getIdx(){return idx.getNat(0);}

	StreamNode mkvet();
	StreamNode format(const char* format);
};


struct StreamNode : Stream {
	Stream* super;

	StreamNode(Stream* super){
		this->super = super;
	}

	StreamNode& put(uint8_t val){this->unit->put_nat(this,val);return *this;}
	StreamNode& put(uint16_t val){this->unit->put_nat(this,val);return *this;}
	StreamNode& put(uint32_t val){this->unit->put_nat(this,val);return *this;}
	StreamNode& put(uint64_t val){this->unit->put_nat(this,val);return *this;}

	StreamNode& put(int8_t val){this->unit->put_int(this,val);return *this;}
	StreamNode& put(int16_t val){this->unit->put_int(this,val);return *this;}
	StreamNode& put(int32_t val){this->unit->put_int(this,val);return *this;}
	StreamNode& put(int64_t val){this->unit->put_int(this,val);return *this;}

	StreamNode& put(float val){this->unit->put_flt(this,val);return *this;}
	StreamNode& put(double val){this->unit->put_flt(this,val);return *this;}

	StreamNode& put(String val){this->putStr(val);return *this;}
	StreamNode& put(void* val, size_t size){this->putRaw(val,size);return *this;}

	Stream& leave(){
		//unit->put_cmd(this);
//cout << unit->toStr() << "\n";
		return *super;
	}
};


struct StreamMsg : Stream {
	StreamMsg() : Stream() {}
	StreamMsg(Unit* unit) : Stream(unit){}
	StreamMsg(Unit& unit) : Stream(unit){}

	void read(){this->unit->put_cmd(this,0);}
};

struct Reader : Stream {
	Reader() : Stream() {}
	Reader(Unit* unit) : Stream(unit){}
	Reader(Unit& unit) : Stream(unit){}

	void readRow(){this->unit->put_cmd(this,0);}
	String toStr(){return this->unit->toStr();}
};

struct Writer : Stream {
	Writer() : Stream() {}
	Writer(Unit* unit) : Stream(unit){}
	Writer(Unit& unit) : Stream(unit){}
};



struct TextSerializer : UnitBasic {
	const char* format;
	size_t i;
	String res;

	TextSerializer(const char* format){
		this->format = format;
		this->i = 0;
	}

	void put_nat(Ctx* ctx, uint64_t val){
		if ( next() ){
			char buf[64]; sprintf(buf,"%lu",val);
			res += buf;
		}
	}

	void put_int(Ctx* ctx, int64_t val){
		if ( next() ){
			char buf[64]; sprintf(buf,"%ld",val);
			res += buf;
		}
	}

	void put_flt(Ctx* ctx, double val){
		if ( next() ){
			char buf[64]; sprintf(buf,"%lf",val);
			res += buf;
		}
	}

	void put_str(Ctx* ctx, String val){
		if ( next() ){
			res += val;
		}
	}


	bool next(){
		while ( true ){
			char c = format[i];
			if ( c == '?' ) {
				i += 1;
				return true;
			} else if ( c == '\0' ){
				return false;
			} else {
				i += 1;
				res += c;
			}
		}
	}

	void openRandom (Random& pack){}
	void openStream (Stream& pack){}
	void get_raw(Ctx* ctx, void* src, size_t size){}
	void get_str(Ctx* ctx, String&   dst){}
	void put_raw(Ctx* ctx, const void* src, size_t size){}

	void put_cmd(Ctx* ctx, int type){
		this->next();
	}

	uint64_t size(){}
	String toStr(){return res;}

};







inline StreamNode Stream::mkvet(){
	StreamNode res(this);
	return res;
}

StreamNode Stream::format(const char* format){
	StreamNode res(this);
	res.unit = new TextSerializer(format);
	return res;
}



struct RandomNode;

struct Random : CtxData {
	Random() : CtxData() {}
	Random(Unit* unit) : CtxData(unit){
		this->type=2;
		this->unit->openRandom(*this);
	}
	Random(Unit& unit) : CtxData(unit){
		this->type=2;
		this->unit->openRandom(*this);
	}

	inline RandomNode& operator[](size_t idx);
};

struct RandomNode : CtxData {
	uint64_t toNat(){
		uint64_t buf; this->unit->get_nat(this,buf);
		return buf;
	}

	int64_t toInt(){
		int64_t buf; this->unit->get_int(this,buf);
		return buf;
	}

	double toFlt(){
		double buf; this->unit->get_flt(this,buf);
		return buf;
	}

	String toStr(){
		String buf; this->unit->get_str(this,buf);
		return buf;
	}

	void putNat(uint64_t val){this->unit->put_nat(this,val);}
	void putInt(int64_t  val){this->unit->put_int(this,val);}
	void putFlt(double   val){this->unit->put_flt(this,val);}
	void putStr(String val){this->unit->put_str(this,val);}

	void operator=(uint8_t val){this->putNat(val);}
	void operator=(uint16_t val){this->putNat(val);}
	void operator=(uint32_t val){this->putNat(val);}
	void operator=(uint64_t val){this->putNat(val);}
	void operator=(int8_t val){this->putInt(val);}
	void operator=(int16_t val){this->putInt(val);}
	void operator=(int32_t val){this->putInt(val);}
	void operator=(int64_t val){this->putInt(val);}
	void operator=(float  val){this->putFlt(val);}
	void operator=(double val){this->putFlt(val);}
	void operator=(String val){this->putStr(val);}

	RandomNode& operator[](size_t idx){
		this->idx.putNat(idx); return *this;
	}
};

inline RandomNode& Random::operator[](size_t idx){
	this->idx.clear();
	this->idx.putNat(idx);
	return *((RandomNode*)this);
}

/*----------------------------------------------------------------------------*/




struct RawBase : Ctx {
	RawBase() : Ctx(){}
	RawBase(Unit& unit) : Ctx(unit){}

	void read(void* dst, size_t size){
		this->unit->get_raw(this,dst,size);
	}

	void write(const void* val, size_t size){
		this->unit->put_raw(this,val,size);
	}
};

struct Raw : RawBase {
	Raw() : RawBase(){}
	Raw(Unit& unit) : RawBase(unit){}

	void blank(){}
	void erase(){}

	Raw& put  (char     val){this->write(&val,sizeof(val));return *this;}
	Raw& put8 (uint8_t  val){this->write(&val,sizeof(val));return *this;}
	Raw& put8 (int8_t   val){this->write(&val,sizeof(val));return *this;}
	Raw& put16(uint16_t val){this->write(&val,sizeof(val));return *this;}
	Raw& put16(int16_t  val){this->write(&val,sizeof(val));return *this;}
	Raw& put32(uint32_t val){this->write(&val,sizeof(val));return *this;}
	Raw& put32(int32_t  val){this->write(&val,sizeof(val));return *this;}
	Raw& put32(float    val){this->write(&val,sizeof(val));return *this;}
	Raw& put64(uint64_t val){this->write(&val,sizeof(val));return *this;}
	Raw& put64(int64_t  val){this->write(&val,sizeof(val));return *this;}
	Raw& put64(double   val){this->write(&val,sizeof(val));return *this;}
	Raw& put  (String   val){this->write(val.c_str(),val.size());return *this;}

	String toStr(){String val; this->unit->get_str(this,val); return val;}

	template <typename T> T get(){
		T res; this->read(&res,sizeof(T));
		return res;
	}

	template <typename T> Raw& put(T val){
		this->write(&val,sizeof(T));
		return *this;
	}

	inline Raw& operator[](size_t idx){this->idx = idx; return *this; }

	inline Raw& operator()(size_t ini,size_t end){
		this->idx = ini; this->idx.putNat(end);
		return *this;
	}
	//inline void operator+=()
};

struct ReaderRaw : RawBase {
	ReaderRaw() : RawBase(){}
	ReaderRaw(Unit& unit) : RawBase(unit){}

	char get(){char c;this->unit->get_raw(this,&c,1);return c;}
};

struct WriterRaw : RawBase {
	ReaderRaw& put();
};



struct Val : Ctx {
	Val() : Ctx(){}
	Val(Unit& unit) : Ctx(unit){}

	inline uint64_t toNat(){uint64_t val;this->unit->get_nat(this,val); return val;}
	inline int64_t  toInt(){int64_t  val;this->unit->get_int(this,val); return val;}
	inline double   toFlt(){double   val;this->unit->get_flt(this,val); return val;}
	inline String   toStr(){String   val;this->unit->get_str(this,val); return val;}

	void setNat(uint64_t val){this->unit->put_nat(this,val);}
	void setInt(int64_t  val){this->unit->put_int(this,val);}
	void setFlt(double   val){this->unit->put_flt(this,val);}
	void setStr(String   val){this->unit->put_str(this,val);}

	void operator=(uint8_t  val){return this->setNat(val);}
	void operator=(uint16_t val){return this->setNat(val);}
	void operator=(uint32_t val){return this->setNat(val);}
	void operator=(uint64_t val){return this->setNat(val);}

	void operator=(int8_t  val){return this->setInt(val);}
	void operator=(int16_t val){return this->setInt(val);}
	void operator=(int32_t val){return this->setInt(val);}
	void operator=(int64_t val){return this->setInt(val);}

	void operator=(float   val){return this->setFlt(val);}
	void operator=(double  val){return this->setFlt(val);}
	void operator=(String  val){return this->setStr(val);}

	inline Raw& operator[](size_t idx){}
};


struct ReaderVal : Ctx {
	ReaderVal() : Ctx(){}
	ReaderVal(Unit& unit) : Ctx(unit){}

	ReaderVal& getNat(uint64_t& val){this->unit->put_nat(this,val);return *this;}
	ReaderVal& getInt(int64_t&  val){this->unit->put_int(this,val);return *this;}
	ReaderVal& getFlt(double&   val){this->unit->put_flt(this,val);return *this;}
	ReaderVal& getStr(String&   val){this->unit->put_str(this,val);return *this;}

	/*WriterVal& get(uint8_t  val){return this->putNat(val);}
	WriterVal& get(uint16_t val){return this->putNat(val);}
	WriterVal& get(uint32_t val){return this->putNat(val);}
	WriterVal& get(uint64_t val){return this->putNat(val);}

	WriterVal& put(int8_t  val){return this->putInt(val);}
	WriterVal& put(int16_t val){return this->putInt(val);}
	WriterVal& put(int32_t val){return this->putInt(val);}
	WriterVal& put(int64_t val){return this->putInt(val);}

	WriterVal& put(float   val){return this->putFlt(val);}
	WriterVal& put(double  val){return this->putFlt(val);}
	WriterVal& put(String  val){return this->putStr(val);}*/

	inline bool next(){}
};

struct WriterVal : Ctx {
	WriterVal() : Ctx(){}
	WriterVal(Unit& unit) : Ctx(unit){}

	WriterVal& putNat(uint64_t val){this->unit->put_nat(this,val);return *this;}
	WriterVal& putInt(int64_t  val){this->unit->put_int(this,val);return *this;}
	WriterVal& putFlt(double   val){this->unit->put_flt(this,val);return *this;}
	WriterVal& putStr(String   val){this->unit->put_str(this,val);return *this;}

	WriterVal& put(uint8_t  val){return this->putNat(val);}
	WriterVal& put(uint16_t val){return this->putNat(val);}
	WriterVal& put(uint32_t val){return this->putNat(val);}
	WriterVal& put(uint64_t val){return this->putNat(val);}

	WriterVal& put(int8_t  val){return this->putInt(val);}
	WriterVal& put(int16_t val){return this->putInt(val);}
	WriterVal& put(int32_t val){return this->putInt(val);}
	WriterVal& put(int64_t val){return this->putInt(val);}

	WriterVal& put(float   val){return this->putFlt(val);}
	WriterVal& put(double  val){return this->putFlt(val);}
	WriterVal& put(String  val){return this->putStr(val);}

	inline void endl(){}
};



struct VetNode;

struct VetPtr : Ctx {
	VetPtr() : Ctx(){}
	VetPtr(Unit& unit) : Ctx(unit){}

	inline Stat   stat(){return Stat(0);}
	inline size_t size(){return this->unit->size();}

	inline VetPtr& putNat(uint64_t val){
		this->unit->put_nat(NULL,val); return *this;
	}

	inline VetPtr& putInt(int64_t val){
		this->unit->put_int(NULL,val); return *this;
	}

	inline VetPtr& putStr(String val){
		this->unit->put_str(NULL,val); return *this;
	}

	inline VetPtr& put(String val){return this->putStr(val);}

	inline VetNode& operator[](size_t idx);
	inline VetNode& operator[](String idx);
};

struct VetNode : Ctx {
	inline Stat   stat(){return Stat(0);}
	inline size_t size(){return 1;}

	uint64_t toNat(){uint64_t val;this->unit->get_nat(this,val); return val;}
	int64_t  toInt(){int64_t  val;this->unit->get_int(this,val); return val;}
	double   toFlt(){double   val;this->unit->get_flt(this,val); return val;}
	String   toStr(){String   val;this->unit->get_str(this,val); return val;}

	void setNat(uint64_t val){this->unit->put_nat(this,val);}
	void setInt(int64_t  val){this->unit->put_int(this,val);}
	void setFlt(double   val){this->unit->put_flt(this,val);}
	void setStr(String   val){this->unit->put_str(this,val);}

	bool isNull(){this->stat().isNull();}
	bool isBlank(){}
	bool isItem(){}
	bool isRaw(){}
	bool isNat(){}
	bool isInt(){}
	bool isFlt(){}
	bool isStr(){}
	bool isColl(){}
	bool isVet(){}
	bool isMap(){}

	void operator=(uint8_t  val){this->setNat(val);}
	void operator=(uint16_t val){this->setNat(val);}
	void operator=(uint32_t val){this->setNat(val);}
	void operator=(uint64_t val){this->setNat(val);}
	void operator=(int8_t   val){this->setInt(val);}
	void operator=(int16_t  val){this->setInt(val);}
	void operator=(int32_t  val){this->setInt(val);}
	void operator=(int64_t  val){this->setInt(val);}
	void operator=(float    val){this->setFlt(val);}
	void operator=(double   val){this->setFlt(val);}
	void operator=(String   val){this->setStr(val);}

	inline VetNode& operator[](size_t idx){this->idx.putNat(idx);return *this;}
	inline VetNode& operator[](String idx){this->idx.putStr(idx);return *this;}
};

inline VetNode& VetPtr::operator[](size_t idx){
	this->idx = idx;
	return *((VetNode*) this);
}

inline VetNode& VetPtr::operator[](String idx){
	return *((VetNode*) this);
}




struct ReaderVet : Ctx {
	ReaderVet() : Ctx(){}
	ReaderVet(Unit& unit) : Ctx(unit){}

	inline VetPtr  row(){}
	inline VetNode operator[](size_t idx){}
	inline VetNode operator[](String idx){}

	void init(){}
	void next(){}
	bool isOk(){}
};

struct WriterVet : VetPtr {
	ReaderRaw& put();
};




struct Map : Ctx {
	inline Map& operator[](size_t idx){}
	inline Map& operator[](String idx){}
};

struct ReaderMap : Map {
	void init(){}
	void next(){}
	bool isOk(){}
};

struct WriterMap : Map {
};
