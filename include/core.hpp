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

#define Log(msg_) printf(("log[%s]: %s\n"), __func__, (msg_))


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
	Stat& setColl(){type[0]='c';return *this;}
	Stat& setVet(){type[0]='c';type[1]='v';type[2]='r';return *this;}
	Stat& setMap(){type[0]='c';type[1]='m';type[2]='r';return *this;}

	Stat& setItem(){type[0]='i';type[2]='r';return *this;}
	Stat& setByte(){setItem();type[1]='b';return *this;}
	Stat& setNat(){setItem();type[1]='n';return *this;}
	Stat& setInt(){setItem();type[1]='i';return *this;}
	Stat& setFlt(){setItem();type[1]='f';return *this;}
	Stat& setStr(){setItem();type[1]='s';return *this;}
	Stat& setRaw(){setItem();type[1]='r';return *this;}

	bool isNull(){return type16==0;}
	bool isPresent(){return type16!=0;}

	bool isRandom(){return type[2]=='r';}
	bool isStream(){return type[2]=='s';}

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
		this->size = 0;
	}

	uint64_t& atNat(int id) const {}
	uint64_t  getNat(int id) const {
		switch ( format[id] ){
			case 'u': return nat64[id];
			case 's': return atoi(str[id].c_str());
			case 'd': return int64[id];
			case 'f': return flt64[id];
		}
		return 0;
	}
	String    getStr(int id) const {
		if ( format[id] == 's' )
			return str[id];
		else if ( format[id] == 'u' ){
			char buf[64]; sprintf(buf,"%lu",nat64[id]);
			return buf;
		} else if ( format[id] == 'd' ){
			char buf[64]; sprintf(buf,"%ld",int64[id]);
			return buf;
		}  else if ( format[id] == 'f' ){
			char buf[64]; sprintf(buf,"%lf",flt64[id]);
			return buf;
		}
		return "";
	}
	void      putNat(uint64_t val){nat64[size]=val;format[size++]='u';}
	void      putStr(String val){str[size] = val;format[size++] = 's';}

	void operator=(uint64_t val){
		size = 1;
		format[0] = 'u';
		nat64[0]  = val;
	}

	void operator=(String val){
		size = 1;
		format[0] = 's';
		str[0]  = val;
	}
};

/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct CtxStream;
struct CtxRandom;

struct Unit {
	size_t ctx_count;
	Unit(){ctx_count=0;}

	// BASIC ======================
	virtual String   classe(){return "";}
	virtual Stat     stat(){return Stat(0);}

	// DATA ======================
	virtual uint64_t size()=0;
	virtual void     root_open_random  (CtxRandom& dst, char mode){}

	// ITEM ======================
	virtual uint64_t toNat()=0;
	virtual int64_t  toInt()=0;
	virtual double   toFlt()=0;
	virtual String   toStr()=0;
	//CtxRandom        toRaw(){CtxRandom res; root_open_random(res,0); return res;}

	// ITEM e VECTOR =============
	virtual void     setNat(uint64_t val)=0;
	virtual void     setInt(int64_t  val)=0;
	virtual void     setFlt(double   val)=0;
	virtual void     setStr(String   val)=0;

	// COLLECTION ================
	virtual void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode)=0;


	// STREAM ======================
	virtual void      root_open_stream (CtxStream& dst, char mode)=0;
	virtual void      node_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode)=0;
	//virtual Stat      stream_stat_line (CtxStream* ctx)=0;
	//virtual void      stream_save_line (CtxStream& ctx, uint8_t level)=0;
	//virtual void      stream_load_line (CtxStream& ctx)=0;

	//virtual Stat     stream_stat (CtxStream* ctx)=0;
	//virtual void     stream_open_stream (CtxStream& dst, char mode)=0;

	/*virtual size_t   stream_get_raw (CtxStream* ctx, void* dst, size_t size)=0;
	virtual size_t   stream_put_raw (CtxStream* ctx, const void* src, size_t size)=0;

	virtual uint64_t stream_get_nat (CtxStream* ctx)=0;
	virtual int64_t  stream_get_int (CtxStream* ctx)=0;
	virtual double   stream_get_flt (CtxStream* ctx)=0;
	virtual void     stream_get_str (CtxStream* ctx, String& dst)=0;

	virtual void     stream_put_nat (CtxStream* ctx, uint64_t val)=0;
	virtual void     stream_put_int (CtxStream* ctx, int64_t  val)=0;
	virtual void     stream_put_flt (CtxStream* ctx, double   val)=0;
	virtual void     stream_put_str (CtxStream* ctx, String   val)=0;*/



	// RANDOM ======================
	virtual Stat     random_stat   (CtxRandom* ctx)=0;
	virtual void     random_erase  (CtxRandom* ctx, bool   index)=0;
	virtual void     random_resize (CtxRandom* ctx, size_t size)=0;
	virtual uint64_t random_size   (CtxRandom* ctx)=0;

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

	//virtual void     random_reduce_idx(CtxRandom& src, CtxRandom& dst)=0;

	Unit* usesUnit(){this->ctx_count += 1; return this;}
	void  freeUnit(){this->ctx_count -= 1;}

	void log(){
		Stat stat = this->stat();
		printf("Unit{class: %s, ptr: %p, type: %s, status: online, ctx: %lu}\n", this->classe().c_str(), this, stat.type, ctx_count);
	}
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/
enum CMD_TYPE { NOT, OPEN, READ, WRITE, CALL};

struct Ctx {
	int    type;
	Unit*  unit;

	union {
		void*   data;
		char    d_char;
		int64_t d_int64;
	};
	Args   base;
	Args   idx;
	Error  err;

	Ctx(){
		this->unit = NULL;
		this->data = NULL;
		type = 0;
	}
	Ctx(Unit* unit){
		this->unit = unit->usesUnit();
		this->data = NULL;
	}
	Ctx(Unit& unit){
		this->unit = unit.usesUnit();
		this->data = NULL;
	}
	Ctx(const Ctx& ctx){
		if ( ctx.unit )
			this->unit = ctx.unit->usesUnit();
		this->data = NULL;
		this->idx  = ctx.idx;
		this->base = ctx.base;
	}

	~Ctx(){
		if ( this->unit ){
			this->unit->freeUnit();
			this->unit = NULL;
		}
	}

	bool isNull()   {return unit==NULL;}
	bool isStream() {return type==1;}
	bool isRandom() {return type==2;}

	size_t getIdxSize(){return this->base.size + this->idx.size;}
};


/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct StreamRawApi {
	void (*read )(CtxStream* ctx, char* dst, size_t size);
	void (*write)(CtxStream* ctx, const char* src, size_t size);
	bool (*is_ok)(CtxStream* ctx);
};

struct StreamValApi {
	void (*get_nat)(CtxStream* ctx);
	void (*get_str)(CtxStream* ctx);
	void (*put_nat)(CtxStream* ctx);
	void (*put_str)(CtxStream* ctx);
};


struct StreamData {
	size_t  ctx_count;
	StreamData(){ctx_count=1;}

	virtual ~StreamData(){}

	virtual size_t raw_get (CtxStream& ctx, char* dst, size_t size){}
	virtual size_t raw_put (CtxStream& ctx, char* dst, size_t size){}
	virtual void   base_update(CtxStream& ctx){}

	virtual uint64_t  val_get_nat (CtxStream& ctx){}
	virtual int64_t   val_get_int (CtxStream& ctx){}
	virtual double    val_get_flt (CtxStream& ctx){}
	virtual String    val_get_str (CtxStream& ctx){}
	virtual CtxRandom val_get_vet (CtxStream& ctx)=0;

	virtual void   val_put_nat (CtxStream& ctx, uint64_t val){}
	virtual void   val_put_int (CtxStream& ctx, int64_t  val){}
	virtual void   val_put_flt (CtxStream& ctx, double   val){}
	virtual void   val_put_str (CtxStream& ctx, String   val){}

	virtual Stat   itr_stat  (CtxStream& ctx){return Stat(0);}
	virtual bool   itr_is_ok (CtxStream& ctx){}
	virtual void   itr_begin (CtxStream& ctx){}
	virtual void   itr_end   (CtxStream& ctx){}
	virtual void   itr_next  (CtxStream& ctx){}
	virtual void   itr_prev  (CtxStream& ctx){}

	virtual void   coll_load(CtxStream& ctx){}
	virtual void   coll_save(CtxStream& ctx){}
	virtual void   coll_row (CtxStream& ctx){}

	StreamData* uses(){this->ctx_count += 1; return this;}
	void free(){
		/*ctx_count -= 1;
		if ( ctx_count == 0 ){
			delete(this);
		}*/
	}
};

struct CtxUnit : Ctx {
	CtxUnit() : Ctx(){}
	CtxUnit(Unit* unit) : Ctx(unit){}
	CtxUnit(Unit& unit) : Ctx(unit){}

	void log(){
		printf("CtxUnit{unit: %p",this->unit);
		if ( this->unit ){
			Stat stat = this->unit->stat();
			printf(", type: %s", stat.type);
		}
		printf("}\n");
	}
};


struct CtxStream : Ctx {
	StreamData* stream_data;
	StreamRawApi raw;
	StreamValApi val;

	CtxStream() : Ctx(){
		stream_data = NULL;
	}
	CtxStream(Unit* unit) : Ctx(unit){
		stream_data = NULL;
	}
	CtxStream(Unit& unit) : Ctx(unit){
		stream_data = NULL;
	}
	CtxStream(const CtxStream& stream){
		this->unit = stream.unit->usesUnit();
		this->raw = stream.raw;
		this->val = stream.val;
	}

	void openStreamData(){this->unit->root_open_stream(*this,0);}

	void log(){
		String res;
		if ( !this->stream_data )
			printf("Ctx{unit: %p, stream: nil}\n",this->unit);
		else {
			this->stream_data->base_update(*this);
			const char* _idx = this->base.getStr(0).c_str();
			printf("Ctx{unit: %p, stream: {key:%s} }\n",this->unit,_idx);
		}
	}
};

struct CtxRandom : Ctx {
	CtxRandom() : Ctx(){}
	CtxRandom(Unit* unit) : Ctx(unit){}
	CtxRandom(Unit& unit) : Ctx(unit){}
	CtxRandom(Unit& unit, size_t idx) : Ctx(unit){this->idx = idx;}
	CtxRandom(Unit& unit, String idx) : Ctx(unit){this->idx = idx;}
	CtxRandom(const CtxUnit& unit) : Ctx(unit){}

	CtxRandom(const CtxRandom& ctx) : Ctx(ctx){}


	Stat stat(){
		return this->unit->random_stat(this);
	}

	uint64_t toNat(){return this->unit->random_get_nat(this);}
	int64_t  toInt(){return this->unit->random_get_int(this);}
	double   toFlt(){return this->unit->random_get_flt(this);}
	String   toStr(){
		String buf; this->unit->random_get_str(this,buf);
		return buf;
	}

	void putNat(uint64_t val){this->unit->random_put_nat(this,val);}
	void putInt(int64_t  val){this->unit->random_put_int(this,val);}
	void putFlt(double   val){this->unit->random_put_flt(this,val);}
	void putStr(String   val){this->unit->random_put_str(this,val);}

	CtxUnit openAsVet(){
		CtxRandom res; this->unit->random_open_random(this,res,0);
		return res.asCtxUnit();
	}

	CtxUnit openAsMap(){
		CtxRandom res; this->unit->random_open_random(this,res,0);
		return res.asCtxUnit();
	}


	void operator=(uint8_t  val){this->putNat(val);}
	void operator=(uint16_t val){this->putNat(val);}
	void operator=(uint32_t val){this->putNat(val);}
	void operator=(uint64_t val){this->putNat(val);}
	void operator=(int8_t   val){this->putInt(val);}
	void operator=(int16_t  val){this->putInt(val);}
	void operator=(int32_t  val){this->putInt(val);}
	void operator=(int64_t  val){this->putInt(val);}
	void operator=(float    val){this->putFlt(val);}
	void operator=(double   val){this->putFlt(val);}
	void operator=(String   val){this->putStr(val);}

	CtxRandom& operator[](size_t idx){this->idx.putNat(idx); return *this;}
	CtxRandom& operator[](String idx){this->idx.putStr(idx); return *this;}

	CtxUnit&   asCtxUnit(){return *((CtxUnit*)this);}


	void log(){
		if ( this->getIdxSize() == 0 ){
			printf("Ctx{unit: %p",this->unit);
			if ( this->unit ){
				Stat stat = this->unit->stat();
				printf(", type: %s", stat.type);
			}
			printf("}\n");
		} else {
			String base_str = base.getStr(0);
			for (int i=1;i<base.size; i++){
				base_str += '/';
				base_str += base.getStr(i);
			}

			String idx_str = idx.getStr(0);
			for (int i=1;i<idx.size; i++){
				idx_str += '/';
				idx_str += idx.getStr(i);
			}

			printf("Ctx{unit: %p, index: %s#%s",this->unit,base_str.c_str(), idx_str.c_str());
			if ( this->unit ){
				Stat stat = this->stat();
				printf(", type: %s", stat.type);
			}
			printf("}\n");
			//const char* _idx = this->idx.getStr(0).c_str();
			//printf("Ctx{unit: %p, stream: {key:%s} }\n",this->unit,_idx);
		}
	}
};


/*----------------------------------------------------------------------------*/



/*============================================================================*/


struct RandomUnit : CtxRandom {
	RandomUnit() : CtxRandom() {}
	RandomUnit(Unit* unit) : CtxRandom(unit){
		this->type=2;
		this->unit->root_open_random(*this, 2);
	}
	RandomUnit(Unit& unit) : CtxRandom(unit){
		this->type=2;
		this->unit->root_open_random(*this, 2);
	}
	RandomUnit(const CtxUnit& unit) : CtxRandom(unit){}
	RandomUnit(const CtxRandom& node) : CtxRandom(node){
		//this->type=2;
		//this->unit->random_open_random(&node,*this, 2);
	}


	Stat stat(){return this->unit->stat();}

	inline CtxRandom operator[](size_t idx){
		return CtxRandom(*this->unit,idx);
	}

	inline CtxRandom operator[](String idx){
		return CtxRandom(*this->unit,idx);
	}
};


struct Reader : CtxStream {
	Reader() : CtxStream(){}
	Reader(Unit& unit) : CtxStream(unit){ this->openStreamData(); }
	Reader(Unit* unit) : CtxStream(unit){ this->openStreamData(); }
	Reader(const CtxRandom& ctx){
		this->unit = ctx.unit;
		ctx.unit->node_open_stream(&ctx,*this,0);
	}

	/*Stat       statRow(){
		return this->stream_data->stat();
	}
	size_t     sizeRow(){return 1;}*/

	Reader& getNat(uint64_t& val){val = this->stream_data->val_get_nat(*this);return *this;}
	Reader& getInt(int64_t&  val){val = this->stream_data->val_get_int(*this);return *this;}
	Reader& getFlt(double&   val){val = this->stream_data->val_get_flt(*this);return *this;}
	Reader& getStr(String&   val){val = this->stream_data->val_get_str(*this);return *this;}

	/*bool read(){
		this->next();
		return this->statRow().isPresent();
	}

	void prev(){this->stream_data->prev(*this);}
	void next(){this->stream_data->next(*this);}
	bool isOk(){return this->stream_data->isOk();}

	void operator++(){this->next();}
	void operator--(){this->prev();}*/
};


struct Iterator : CtxStream {
	Iterator() : CtxStream(){}
	Iterator(Unit& unit) : CtxStream(unit){
		this->openStreamData();
		this->stream_data->itr_begin(*this);
	}
	Iterator(Unit* unit) : CtxStream(unit){
		this->openStreamData();
		this->stream_data->itr_begin(*this);
	}
	Iterator(const CtxRandom& ctx){
		this->unit = ctx.unit;
		ctx.unit->node_open_stream(&ctx,*this,0);
		this->stream_data->itr_begin(*this);
	}

	Stat      stat(){return this->stream_data->itr_stat(*this);}

	uint64_t  toNat(){return this->stream_data->val_get_nat(*this);}
	int64_t   toInt(){return this->stream_data->val_get_int(*this);}
	double    toFlt(){return this->stream_data->val_get_flt(*this);}
	String    toStr(){return this->stream_data->val_get_str(*this);}
	CtxRandom toRaw(){}
	CtxRandom toVet(){return this->stream_data->val_get_vet(*this);}
	CtxRandom toMap(){}

	Iterator& getNat(uint64_t& val){val =this->toNat();return *this;}
	Iterator& getInt(int64_t&  val){val =this->toInt();return *this;}
	Iterator& getFlt(double&   val){val =this->toFlt();return *this;}
	Iterator& getStr(String&   val){val+=this->toStr();return *this;}

	Iterator& putNat(uint64_t val){this->stream_data->val_put_nat(*this,val);return *this;}
	Iterator& putInt(int64_t  val){this->stream_data->val_put_int(*this,val);return *this;}
	Iterator& putFlt(double   val){this->stream_data->val_put_flt(*this,val);return *this;}
	Iterator& putStr(String   val){this->stream_data->val_put_str(*this,val);return *this;}

	void prev(){this->stream_data->itr_prev(*this);}
	void next(){this->stream_data->itr_next(*this);}
	bool isOk(){return this->stream_data->itr_is_ok(*this);}

	void operator++(){this->next();}
	void operator--(){this->prev();}

	String key(){return this->idx.getStr(0);}
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
	ReaderRaw(const CtxStream& ctx) : CtxStream(ctx){}
	char getc(){
		char c; this->raw.read(this,&c,1);
		return c;
	}
	bool isOk(){return this->raw.is_ok(this);}
};


struct WriterRaw : CtxStream {
	WriterRaw() : CtxStream(){}
	WriterRaw(Unit& unit) : CtxStream(unit){}
	WriterRaw(const CtxStream& ctx) : CtxStream(ctx){}

	WriterRaw& put(char     val){this->write(&val,sizeof(val));return *this;}
	WriterRaw& put(uint8_t  val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(uint16_t val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(uint32_t val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(uint64_t val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(int8_t  val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(int16_t val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(int32_t val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(int64_t val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(float   val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(double  val){this->write((char*)&val,sizeof(val));return *this;}
	WriterRaw& put(String val){this->write(val.c_str(), val.size());return *this;}

	size_t write(const char* src, size_t size){
		this->raw.write(this,src,size);
	}
	bool isOk(){return this->raw.is_ok(this);}
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

	size_t write(const void* val, size_t size){}
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
};

struct WriterVal : CtxStream {
	WriterVal() : CtxStream(){}
	WriterVal(Unit& unit) : CtxStream(unit){}
};

/*----------------------------------------------------------------------------*/


/*============================================================================*/

/*struct VetNode : CtxRandom {
	inline VetNode& operator[](size_t idx){this->idx.putNat(idx);return *this;}
	inline VetNode& operator[](String idx){this->idx.putStr(idx);return *this;}
};*/


struct VetPtr : RandomUnit {
	VetPtr() : RandomUnit(){}
	VetPtr(Unit& unit) : RandomUnit(unit){}
	VetPtr(const CtxUnit&   unit) : RandomUnit(unit){}
	VetPtr(const CtxRandom& node) : RandomUnit(node){}

	inline Stat   stat(){return this->unit->stat();}
	inline size_t size(){return this->unit->size();}

	inline VetPtr& putNat(uint64_t val){}
	inline VetPtr& putInt(int64_t  val){}
	inline VetPtr& putStr(String   val){}
	inline VetPtr& put   (String   val){}
};


struct ReaderVet : Reader {
	ReaderVet() : Reader(){}
	ReaderVet(Unit&   unit) : Reader(unit){}
	ReaderVet(VetPtr& unit) : Reader(unit){}

	/*inline VetPtr  row(){}
	inline CtxRandom operator[](size_t idx){}
	inline CtxRandom operator[](String idx){}*/
};

struct WriterVet : CtxStream {
	ReaderRaw& put();
};

/*----------------------------------------------------------------------------*/



/*============================================================================*/

struct MapNode : CtxRandom {
	MapNode(Unit& unit) : CtxRandom(unit){}
	MapNode(Unit* unit) : CtxRandom(unit){}
	MapNode(RandomUnit& ptr, String idx) : CtxRandom(ptr.unit){
		this->idx = idx;
	}
	MapNode(RandomUnit& ptr, size_t idx) : CtxRandom(ptr.unit){
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

	Iterator begin(){
		Iterator it(*this);
		return it;
	}

	inline MapNode operator[](size_t idx){return MapNode(*this,idx);}
	inline MapNode operator[](String idx){return MapNode(*this,idx);}
};

struct ReaderMap : Reader {
};

struct WriterMap : CtxStream {
};

typedef MapPtr TreePtr;

/*----------------------------------------------------------------------------*/











/*============================================================================*/

struct UnitItem : Unit {
	CtxStream toStream(){
		CtxStream res(this); this->root_open_stream(res,0);
		return res;
	}
};


struct UnitStream : Unit {
	void node_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode)=0;
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

	CtxRandom operator[](size_t idx){return CtxRandom(*this,idx);}
	CtxRandom operator[](String idx){return CtxRandom(*this,idx);}
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

	Iterator begin(){return Iterator(*this);}

	MapNode operator[](String idx){
		MapNode res(*this);
		res.idx = idx;
		return res;
	}
};

/*----------------------------------------------------------------------------*/
