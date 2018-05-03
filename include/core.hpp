/*============================================================================*/

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

typedef std::string String;

using namespace std;

/*----------------------------------------------------------------------------*/


/*============================================================================*/

#define LOG(msg_) printf(("log[%s]: %s\n"), __func__, (msg_))


/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct Stat {
	union {
		char    type[4];
		int32_t type16;
	};

	Stat(){this->type16=0;}
	Stat(int v){this->type16=v;}

	Stat& setNull(){type16 = 0;return *this;}
	Stat& setVet(){type[0]='c';type[1]='v';return *this;}
	Stat& setMap(){type[0]='c';type[1]='m';return *this;}

	Stat& setItem(){type[0]='i';return *this;}
	Stat& setByte(){setItem();type[1]='b';return *this;}
	Stat& setNat(){setItem();type[1]='n';return *this;}
	Stat& setInt(){setItem();type[1]='i';return *this;}
	Stat& setFlt(){setItem();type[1]='f';return *this;}
	Stat& setStr(){setItem();type[1]='s';return *this;}
	Stat& setRaw(){setItem();type[1]='r';return *this;}

	bool isNull(){return type16==0;}
	bool isPresent(){return type16!=0;}

	bool isRandom(){return false;}

	bool isColl(){return type[0]=='c';}
	bool isVet(){return isColl() && type[1]=='v';}
	bool isMap(){return isColl() && type[1]=='m';}

	bool isItem(){return type[0]=='i';}
	bool isByte(){return isItem() && type[1]=='b';}
	bool isNat(){return isItem() && type[1]=='n';}
	bool isInt(){return isItem() && type[1]=='i';}
	bool isFlt(){return isItem() && type[1]=='f';}
	bool isStr(){return isItem() && type[1]=='s';}
	bool isRaw(){return isItem() && type[1]=='r';}

	// Friend Function
	friend std::ostream& operator<<(std::ostream& out, Stat stat){
		out << stat.type[0] << stat.type[1];
		return out;
	}
};

struct Error {
	void putWarn(String msg){
		std::cout << msg << endl;
	}

	void putFatal(String msg){
		std::cout << msg << endl;
		exit(1);
	}
};


/*----------------------------------------------------------------------------*/



/*============================================================================*/

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

	uint64_t getNat(int id) const {
		size_t idx = base + id;
		switch ( format[idx] ){
			case 'u': return nat64[idx];
			case 's': return atoi(str[id].c_str());
			case 'd': return int64[idx];
			case 'f': return flt64[idx];
		}
		return 0;
	}

	String   getStr(const int id) const {
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

	void operator=(String val){
		base = 0; size = 1;
		format[0] = 's';
		str[0]  = val;
	}

	uint64_t nextNat(){
		uint64_t val = this->getNat(0); this->next();
		return val;
	}

	void next(){this->base += 1;}
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct Unit;

struct Ctx {
	int    type;
	Unit*       unit;

	union {
		void*   data;
		char    d_char;
		int64_t d_int64;
	};
	Args   idx;
	Error  err;

	Ctx(){type = 0;}
	Ctx(Unit* unit){this->unit = unit;}
	Ctx(Unit& unit){this->unit = &unit;}

	bool isNull(){return unit==NULL;}
	bool isStream(){return type==1;}
	bool isRandom(){return type==2;}
	bool isCtxRoot(){return type==3;}
	bool isCtxNode(){return type==4;}
};


struct StreamData {
	virtual ~StreamData()=0;
	virtual void begin()=0;
	virtual void prev(Ctx& ctx)=0;
	virtual void next(Ctx& ctx)=0;
	virtual bool isOk()=0;
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct CtxStream;
struct CtxRandom;

struct Unit {
	// BASIC
	virtual Stat  stat(){return Stat(0);}

	// ITEM ======================
	virtual uint64_t toNat()=0;
	virtual int64_t  toInt()=0;
	virtual double   toFlt()=0;
	virtual String   toStr()=0;

	virtual void     setNat(uint64_t val)=0;
	virtual void     setInt(int64_t  val)=0;
	virtual void     setFlt(double   val)=0;
	virtual void     setStr(String   val)=0;

	virtual uint64_t size()=0;

	// Collection
	//virtual void     root_open_stream (CtxStream& dst, char mode)=0;

	// STREAM ======================
	//virtual Stat     stream_stat (CtxStream* ctx)=0;
	//virtual void     stream_open_stream (CtxStream& dst, char mode)=0;

	virtual size_t   stream_get_raw (CtxStream* ctx, void* dst, size_t size)=0;
	virtual size_t   stream_put_raw (CtxStream* ctx, const void* src, size_t size)=0;

	virtual Stat      stream_stat_line (CtxStream* ctx)=0;
	virtual void      stream_save_line (CtxStream* ctx, uint8_t level)=0;
	virtual void      stream_load_line (CtxStream* ctx)=0;
	virtual CtxRandom stream_get_line  (CtxStream* ctx)=0;


	virtual uint64_t stream_get_nat (CtxStream* ctx)=0;
	virtual int64_t  stream_get_int (CtxStream* ctx)=0;
	virtual double   stream_get_flt (CtxStream* ctx)=0;
	virtual void     stream_get_str (CtxStream* ctx, String& dst)=0;

	virtual void     stream_put_nat (CtxStream* ctx, uint64_t val)=0;
	virtual void     stream_put_int (CtxStream* ctx, int64_t  val)=0;
	virtual void     stream_put_flt (CtxStream* ctx, double   val)=0;
	virtual void     stream_put_str (CtxStream* ctx, String   val)=0;



	// RANDOM ======================
	virtual Stat     random_stat   (CtxRandom* ctx)=0;
	virtual void     random_erase  (CtxRandom* ctx, bool   index)=0;
	virtual void     random_resize (CtxRandom* ctx, size_t size)=0;
	virtual uint64_t random_size   (CtxRandom* ctx)=0;

	virtual void random_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode)=0;
	virtual void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode)=0;

	virtual uint64_t random_get_raw (CtxRandom* ctx, void* dst, size_t size)=0;
	virtual uint64_t random_put_raw (CtxRandom* ctx, const void* src, size_t size)=0;

	virtual uint64_t random_get_nat (CtxRandom* ctx)=0;
	virtual int64_t  random_get_int (CtxRandom* ctx)=0;
	virtual double   random_get_flt (CtxRandom* ctx)=0;
	virtual void     random_get_str (CtxRandom* ctx, String& dst)=0;

	virtual void     random_put_nat  (CtxRandom* ctx, uint64_t val)=0;
	virtual void     random_put_int  (CtxRandom* ctx, int64_t  val)=0;
	virtual void     random_put_flt  (CtxRandom* ctx, double   val)=0;
	virtual void     random_put_str  (CtxRandom* ctx, String   val)=0;

	virtual void     random_make      (CtxRandom* ctx, Stat   type)=0;
	virtual void     random_make_item (CtxRandom* ctx, size_t size)=0;
};


struct CtxStream : Ctx {
	CtxStream() : Ctx(){}
	CtxStream(Unit* unit) : Ctx(unit){}
	CtxStream(Unit& unit) : Ctx(unit){}

	void init(){this->unit->random_open_stream(NULL,*this,0);}
};

struct CtxRandom : Ctx {
	CtxRandom() : Ctx(){}
	CtxRandom(Unit* unit) : Ctx(unit){}
	CtxRandom(Unit& unit) : Ctx(unit){}
	CtxRandom(CtxRandom* ctx) : Ctx(unit){}
};


/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct RandomNode : CtxRandom {
	RandomNode(Unit* unit) : CtxRandom(unit) {}
	RandomNode(Unit& unit) : CtxRandom(unit) {}
	RandomNode(CtxRandom* ctx) : CtxRandom(ctx) {}

	RandomNode(CtxRandom* ctx, size_t idx) : CtxRandom(ctx) {
		this->idx = idx;
	}

	RandomNode(CtxRandom* ctx, String idx) : CtxRandom(ctx) {
		this->idx = idx;
	}

	uint64_t toNat(){
		return this->unit->random_get_nat(this);
	}

	int64_t toInt(){
		return this->unit->random_get_int(this);
	}

	double toFlt(){
		return this->unit->random_get_flt(this);
	}

	String toStr(){
		String buf; this->unit->random_get_str(this,buf);
		return buf;
	}

	void putNat(uint64_t val){this->unit->random_put_nat(this,val);}
	void putInt(int64_t  val){this->unit->random_put_int(this,val);}
	void putFlt(double   val){this->unit->random_put_flt(this,val);}
	void putStr(String val){this->unit->random_put_str(this,val);}

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

	RandomNode& operator[](String idx){
		this->idx.putStr(idx); return *this;
	}
};

struct RandomUnit : CtxRandom {
	RandomUnit() : CtxRandom() {}
	RandomUnit(Unit* unit) : CtxRandom(unit){
		this->type=2;
		this->unit->random_open_random(NULL, *this, 2);
	}
	RandomUnit(Unit& unit) : CtxRandom(unit){
		this->type=2;
		this->unit->random_open_random(NULL, *this, 2);
	}

	inline RandomNode operator[](size_t idx){
		return RandomNode(this,idx);
	}

	inline RandomNode operator[](String idx){
		return RandomNode(this,idx);
	}
};

struct Reader : CtxStream {
	Reader() : CtxStream(){}
	Reader(Unit& unit) : CtxStream(unit){ this->init(); }
	Reader(Unit* unit) : CtxStream(unit){ this->init(); }
	Reader(CtxRandom& ctx){
		this->unit = ctx.unit;
		ctx.unit->random_open_stream(&ctx,*this,0);
	}



	Stat       statRow(){return this->unit->stream_stat_line(this);}
	size_t     sizeRow(){return 1;}

	Reader& getNat(uint64_t& val){val = this->unit->stream_get_nat(this);return *this;}
	Reader& getInt(int64_t&  val){val = this->unit->stream_get_int(this);return *this;}
	Reader& getFlt(double&   val){val = this->unit->stream_get_flt(this);return *this;}
	Reader& getStr(String&   val){this->unit->stream_get_str(this,val);return *this;}

	bool read(){
		this->unit->stream_load_line(this);
		return this->unit->stream_stat_line(this).isPresent();
	}
	void next(){this->unit->stream_load_line(this);}
	bool isOk(){return this->unit->stream_stat_line(this).isPresent();}
};


/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct RawNode : CtxRandom {
	void blank(){}
	void erase(){}

	void put8 (char        val){this->write(&val,sizeof(val));}
	void put8 (const char* val){this->write(val,strlen(val));}
	void put8 (String      val){this->write(val.c_str(),val.size());}
	void put8 (uint8_t  val){this->write(&val,sizeof(val));}
	void put8 (int8_t   val){this->write(&val,sizeof(val));}
	void put16(uint16_t val){this->write(&val,sizeof(val));}
	void put16(int16_t  val){this->write(&val,sizeof(val));}
	void put32(uint32_t val){this->write(&val,sizeof(val));}
	void put32(int32_t  val){this->write(&val,sizeof(val));}
	void put32(float    val){this->write(&val,sizeof(val));}
	void put64(uint64_t val){this->write(&val,sizeof(val));}
	void put64(int64_t  val){this->write(&val,sizeof(val));}
	void put64(double   val){this->write(&val,sizeof(val));}


	size_t read(void* dst, size_t size){
		return this->unit->random_get_raw(this,dst,size);
	}

	size_t write(const void* val, size_t size){
		return this->unit->random_put_raw(this,val,size);
	}

	template <typename T> T getAs(){
		T res; this->read(&res,sizeof(T));
		return res;
	}

	template <typename T> void putAs(T val){
		this->write(&val,sizeof(T));
		return *this;
	}
};


struct Raw : RandomUnit {
	Raw() : RandomUnit(){}
	Raw(Unit& unit) : RandomUnit(unit){}

	Raw(const CtxRandom& random){
		this->unit = random.unit;
	}

	String toStr(){String val; this->unit->random_get_str(this,val); return val;}

	inline RawNode& operator[](size_t idx){
		this->idx = idx;
		return *(RawNode*)this;
	}

	inline RawNode& operator()(size_t ini,size_t end){
		this->idx = ini; this->idx.putNat(end);
		return *(RawNode*)this;
	}
};


struct ReaderRaw : CtxStream {
	ReaderRaw() : CtxStream(){}
	ReaderRaw(Unit& unit) : CtxStream(unit){}

	char get(){char c;this->unit->stream_get_raw(this,&c,1);return c;}
};


struct WriterRaw : CtxStream {
	WriterRaw() : CtxStream(){}
	WriterRaw(Unit& unit) : CtxStream(unit){}

	WriterRaw& put(char     val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(uint8_t  val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(uint16_t val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(uint32_t val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(uint64_t val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(int8_t  val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(int16_t val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(int32_t val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(int64_t val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(float   val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(double  val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(String val){this->write(val.c_str(), val.size());return *this;}

	size_t write(const void* val, size_t size){
		return this->unit->stream_put_raw(this,val,size);
	}
};


struct WriterText : CtxStream {
	WriterText() : CtxStream(){}
	WriterText(Unit& unit) : CtxStream(unit){}

	WriterText& put(uint8_t  val){
		char buf[64]; sprintf(buf,"%u",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(uint16_t val){
		char buf[64]; sprintf(buf,"%u",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(uint32_t val){
		char buf[64]; sprintf(buf,"%u",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(uint64_t val){
		char buf[64]; sprintf(buf,"%lu",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(int8_t  val){
		char buf[64]; sprintf(buf,"%d",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(int16_t val){
		char buf[64]; sprintf(buf,"%d",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(int32_t val){
		char buf[64]; sprintf(buf,"%d",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(int64_t val){
		char buf[64]; sprintf(buf,"%ld",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(float   val){
		char buf[64]; sprintf(buf,"%f",val);
		this->write(buf,strlen(buf));return *this;
	}
	WriterText& put(double  val){
		char buf[64]; sprintf(buf,"%lf",val);
		this->write(buf,strlen(buf));return *this;
	}

	WriterText& put(String val){this->write(val.c_str(), val.size());return *this;}

	size_t write(const void* val, size_t size){
		return this->unit->stream_put_raw(this,val,size);
	}
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct Val : Ctx {
	Val() : Ctx(){}
	Val(Unit& unit) : Ctx(unit){}

	inline uint64_t toNat(){return this->unit->toNat();}
	inline int64_t  toInt(){return this->unit->toInt();}
	inline double   toFlt(){return this->unit->toFlt();}
	inline String   toStr(){return this->unit->toStr();}

	void setNat(uint64_t val){this->unit->setNat(val);}
	void setInt(int64_t  val){this->unit->setInt(val);}
	void setFlt(double   val){this->unit->setFlt(val);}
	void setStr(String   val){this->unit->setStr(val);}

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


struct ReaderVal : Reader {
	ReaderVal() : Reader(){}
	ReaderVal(Unit& unit) : Reader(unit){}
	ReaderVal(CtxRandom& ctx) : Reader(ctx){}

	Stat       statRow(){return this->unit->stream_stat_line(this);}
	size_t     sizeRow(){return 1;}

	ReaderVal& getNat(uint64_t& val){val = this->unit->stream_get_nat(this);return *this;}
	ReaderVal& getInt(int64_t&  val){val = this->unit->stream_get_int(this);return *this;}
	ReaderVal& getFlt(double&   val){val = this->unit->stream_get_flt(this);return *this;}
	ReaderVal& getStr(String&   val){this->unit->stream_get_str(this,val);return *this;}

	bool read(){
		this->unit->stream_load_line(this);
		return this->unit->stream_stat_line(this).isPresent();
	}
	void next(){this->unit->stream_load_line(this);}
	bool isOk(){return this->unit->stream_stat_line(this).isPresent();}
};


struct WriterVal : CtxStream {
	WriterVal() : CtxStream(){}
	WriterVal(Unit& unit) : CtxStream(unit){}

	WriterVal& putNat(uint64_t val){this->unit->stream_put_nat(this,val);return *this;}
	WriterVal& putInt(int64_t  val){this->unit->stream_put_int(this,val);return *this;}
	WriterVal& putFlt(double   val){this->unit->stream_put_flt(this,val);return *this;}
	WriterVal& putStr(String   val){this->unit->stream_put_str(this,val);return *this;}

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

	inline void endl(){this->unit->stream_save_line(this,1);}
};

/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct VetNode : CtxRandom {
	inline Stat   stat(){return Stat(0);}
	inline size_t size(){return 1;}

	uint64_t toNat(){return this->unit->random_get_nat(this);}
	int64_t  toInt(){return this->unit->random_get_int(this);}
	double   toFlt(){return this->unit->random_get_flt(this);}
	String   toStr(){String   val;this->unit->random_get_str(this,val); return val;}

	void setNat(uint64_t val){this->unit->random_put_nat(this,val);}
	void setInt(int64_t  val){this->unit->random_put_int(this,val);}
	void setFlt(double   val){this->unit->random_put_flt(this,val);}
	void setStr(String   val){this->unit->random_put_str(this,val);}

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


struct VetPtr : RandomUnit {
	VetPtr() : RandomUnit(){}
	VetPtr(Unit& unit) : RandomUnit(unit){}

	inline Stat   stat(){return Stat(0);}
	inline size_t size(){return this->unit->size();}

	inline VetPtr& putNat(uint64_t val){
		this->unit->stream_put_nat(NULL,val); return *this;
	}

	inline VetPtr& putInt(int64_t val){
		this->unit->stream_put_int(NULL,val); return *this;
	}

	inline VetPtr& putStr(String val){
		this->unit->stream_put_str(NULL,val); return *this;
	}

	inline VetPtr& put(String val){return this->putStr(val);}
};


struct ReaderVet : CtxStream {
	ReaderVet() : CtxStream(){}
	ReaderVet(Unit& unit) : CtxStream(unit){}

	inline VetPtr  row(){}
	inline VetNode operator[](size_t idx){}
	inline VetNode operator[](String idx){}

	void init(){}
	void next(){}
	bool isOk(){}
};

struct WriterVet : CtxStream {
	ReaderRaw& put();
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct MapNode : RandomNode {
	MapNode(Unit& unit) : RandomNode(unit){}
	MapNode(Unit* unit) : RandomNode(unit){}
	MapNode(RandomUnit& ptr, String idx) : RandomNode(ptr.unit){
		this->idx = idx;
	}
	MapNode(RandomUnit& ptr, size_t idx) : RandomNode(ptr.unit){
		this->idx = idx;
	}

	inline CtxRandom open(){
		CtxRandom dst;
		this->unit->random_open_random(this,dst,0);
		return dst;
	}

	inline void mkitem(size_t size=0){this->unit->random_make_item(this,size);}
	inline void mkmap(){this->unit->random_make(this,Stat().setMap());}
	inline void mkvet(){this->unit->random_make(this,Stat().setVet());}

	inline MapNode& operator[](size_t idx){
		this->idx.putNat(idx);
		return *this;
	}

	inline MapNode& operator[](String idx){
		this->idx.putStr(idx);
		return *this;
	}
};


struct MapPtr : RandomUnit {
	MapPtr() : RandomUnit(){}
	MapPtr(Unit& unit) : RandomUnit(unit){}
	MapPtr(Unit* unit) : RandomUnit(unit){}

	MapPtr(const MapNode& node) : RandomUnit(node.unit){
		node.unit->random_open_random( (CtxRandom*)&node,*this,0 );
	}

	Stat stat(){
		if ( this->idx.size == 0 ){
			return this->unit->stat();
		} else {
			return this->unit->random_stat(this);
		}
	}

	inline MapNode& root(){
		this->idx.clear();
		return *((MapNode*)this);
	}

	inline MapNode operator[](size_t idx){return MapNode(*this,idx);}
	inline MapNode operator[](String idx){return MapNode(*this,idx);}
};

struct ReaderMap : Reader {
	void next(){this->unit->stream_load_line(this);}
	bool isOk(){return this->unit->stream_stat_line(this).isPresent();}
};

struct WriterMap : CtxStream {
	void next(){this->unit->stream_save_line(this,1);}
};

typedef MapPtr TreePtr;

/*----------------------------------------------------------------------------*/











/*============================================================================*/


struct UnitStream : Unit {
	void random_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode)=0;
	void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){}

	size_t   random_get_raw (CtxRandom* ctx, void* dst, size_t size){}
	size_t   random_put_raw (CtxRandom* ctx, const void* src, size_t size){}

	uint64_t random_get_nat (CtxRandom* ctx){return 0;}
	int64_t  random_get_int (CtxRandom* ctx){return 0;}
	double   random_get_flt (CtxRandom* ctx){return 0;}
	void     random_get_str (CtxRandom* ctx, String& dst){}

	void random_put_nat  (CtxRandom* ctx, uint64_t val){}
	void random_put_int  (CtxRandom* ctx, int64_t  val){}
	void random_put_flt  (CtxRandom* ctx, double   val){}
	void random_put_str  (CtxRandom* ctx, String   val){}

	Stat random_stat   (CtxRandom* ctx){return Stat(0);}
	void random_erase  (CtxRandom* ctx, bool   index){}
	void random_resize (CtxRandom* ctx, size_t size){}
	uint64_t random_size   (CtxRandom* ctx){return 0;}

	virtual void random_make      (CtxRandom* ctx, Stat   type){}
	virtual void random_make_item (CtxRandom* ctx, size_t size){}


	uint64_t toNat(){return 0;}
	int64_t  toInt(){return 0;}
	double   toFlt(){return 0;}
	String   toStr(){return "";}

	void setNat(uint64_t val){}
	void setInt(int64_t  val){}
	void setFlt(double   val){}
	void setStr(String   val){}

	//uint64_t rawsize(){return 0;}
	uint64_t size(){return 0;}
};



struct UnitCollection : Unit {
	uint64_t toNat(){return 0;}
	int64_t  toInt(){return 0;}
	double   toFlt(){return 0;}
	String   toStr(){return "";}

	void setNat(uint64_t val){}
	void setInt(int64_t  val){}
	void setFlt(double   val){}
	void setStr(String   val){}
};


struct UnitVector : UnitCollection {
	Stat stream_stat      (CtxStream* ctx){return Stat(0);}

	void random_open_stream (CtxRandom* ctx, CtxStream& dst, char mode){
		//if ( mode == 0 ){
			dst.idx = 0;
		//} else if ( mode == 1)
		//	dst.idx = this->unit->size();
	}

	size_t   stream_get_raw (CtxStream* ctx, void* dst, size_t size){
		size_t read = this->random_get_raw( (CtxRandom*)ctx,dst,size );
		ctx->idx.atNat(0) += read;
	}

	size_t   stream_put_raw (CtxStream* ctx, const void* src, size_t size){
		size_t wrote = this->random_put_raw( (CtxRandom*)ctx,src,size );
		ctx->idx.atNat(0) += wrote;
	}

	uint64_t stream_get_nat (CtxStream* ctx){
		uint64_t res = this->random_get_nat( (CtxRandom*)ctx );
		ctx->idx.atNat(0) += 1;
		return res;
	}

	int64_t  stream_get_int (CtxStream* ctx){
		int64_t res = this->random_get_int( (CtxRandom*)ctx );
		ctx->idx.atNat(0) += 1;
		return res;
	}

	double   stream_get_flt (CtxStream* ctx){
		double res = this->random_get_flt( (CtxRandom*)ctx );
		ctx->idx.atNat(0) += 1;
		return res;
	}

	void     stream_get_str (CtxStream* ctx, String& dst){
		this->random_get_str( (CtxRandom*)ctx, dst );
		ctx->idx.atNat(0) += 1;
	}

	void stream_put_nat  (CtxStream* ctx, uint64_t val){
		this->random_put_nat( (CtxRandom*)ctx, val );
		ctx->idx.atNat(0) += 1;
	}

	void stream_put_int  (CtxStream* ctx, int64_t  val){
		this->random_put_int( (CtxRandom*)ctx, val );
		ctx->idx.atNat(0) += 1;
	}

	void stream_put_flt  (CtxStream* ctx, double   val){
		this->random_put_flt( (CtxRandom*)ctx, val );
		ctx->idx.atNat(0) += 1;
	}

	void stream_put_str  (CtxStream* ctx, String   val){
		this->random_put_str( (CtxRandom*)ctx, val );
		ctx->idx.atNat(0) += 1;
	}
};


struct UnitTree : UnitCollection {

	// Macros
	inline void mkmap(String idx){
		MapPtr ctx = this;
		//ctx[idx].mkmap();
	}

	inline void mkvet(String idx){
		MapPtr ctx = this;
		//ctx[idx].mkvet();
	}

	inline MapNode operator[](String idx){
		//return MapNode(*this,idx);
	}
};

/*----------------------------------------------------------------------------*/
