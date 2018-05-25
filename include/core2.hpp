/*============================================================================*/

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

typedef std::string String;

using namespace std;

struct Unit;
struct Ctx;
struct CtxRandom;

/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct Num {
	union {
		uint64_t nat64;
		int64_t  int64;
		double   flt64;
	};
	Num(){}
	Num(uint8_t  val){this->nat64 = val;}
	Num(uint16_t val){this->nat64 = val;}
	Num(uint32_t val){this->nat64 = val;}
	Num(int8_t   val){this->int64 = val;}
	Num(int16_t  val){this->int64 = val;}
	Num(int32_t  val){this->int64 = val;}
	Num(float    val){this->flt64 = val;}
	Num(double   val){this->flt64 = val;}
};

struct UnitRef {
	Unit*  unit;
	//VetVal base;
};

struct UnitStack {
	UnitRef super;
	UnitRef cur;
};

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


/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct Unit {
	virtual Stat    root_stat()=0;
	virtual size_t  root_size()=0;
	//virtual size_t  root_size_raw()=0;
	virtual void    root_clear()=0;

	virtual size_t  root_get_raw(char* dst, size_t size)=0;
	virtual Num     root_get_num(char type)=0;
	virtual void    root_get_str(String& out)=0;
	virtual UnitRef root_get_col()=0;

	virtual size_t  root_put_raw(const char* dst, size_t size)=0;
	virtual void    root_put_num(char type, Num val)=0;
	virtual void    root_put_str(String val)=0;
	virtual void    root_put_col(Ctx& src)=0;

	virtual void    root_end_line()=0;


	virtual void    node_stat(CtxRandom& ctx)=0;
	virtual size_t  node_size(CtxRandom& ctx)=0;
	virtual void    node_clear(CtxRandom& ctx)=0;

	virtual size_t  node_get_raw(CtxRandom& ctx, char* dst, size_t size)=0;
	virtual Num     node_get_num(CtxRandom& ctx, char type)=0;
	virtual void    node_get_str(CtxRandom& ctx, String& out)=0;
	virtual UnitRef node_get_col(CtxRandom& ctx)=0;

	virtual size_t  node_put_raw(CtxRandom& ctx, const char* dst, size_t size)=0;
	virtual void    node_put_num(CtxRandom& ctx, char type, Num val)=0;
	virtual void    node_put_str(CtxRandom& ctx, String val)=0;
	virtual void    node_put_col(CtxRandom& ctx, Ctx& src)=0;

	//virtual void   node_idx2node(CtxRandom& ctx)=0;
};





struct Pipe;

struct Value {
	int type;
	union {
		uint64_t nat64;
		int64_t  int64;
		double   flt64;
		Pipe*     pipe;
	};
	String   str;
	UnitRef unit;

	Value(){}
	Value(int zero){this->type=0;}

	void operator=(int        val){this->type=1;this->int64=val;}
	void operator=(float      val){this->type=2;this->flt64=val;}
	void operator=(String     val){this->type=3;this->str=val;}
	void operator=(Pipe*      val){this->type=4;this->pipe=val;}

	size_t size();
	String toStr();
};


struct Pipe {
	Pipe*  super;
	size_t reader;
	size_t size;
	Value  data[8];

	Pipe(){this->super=NULL;this->clear();}
	Pipe(Pipe* super){this->super=super;this->clear();}
	void clear(){size=reader=0;}

	Pipe& put(int    val){this->data[size++]=val;return *this;}
	Pipe& put(String val){this->data[size++]=val;return *this;}
	Pipe& put(Pipe&  val){this->data[size++]=&val;return *this;}

	Value get(){
		if ( reader < size )
			return this->data[reader++];
		else
			return Value(0);
	}

	Pipe& enter(){
		Pipe* res = new Pipe(this);
		return *res;
	}

	Pipe& leave(){
		this->super->put(*this);
		return *this->super;
	}

	String toStr(){
		String res = "[";
		for (int i=0; i<size; i++){
			res += data[i].toStr();
			res += ';';
		}
		res += ']';
		return res;
	}
};


String Value::toStr(){
	if ( type == 0 )
		return "[NULL]";
	if ( type == 1 ){
		char buf[64]; sprintf(buf,"%ld",int64);
		return buf;
	}
	if ( type == 4 ){
		return this->pipe->toStr();
	}
	return str;
}

size_t Value::size(){
	if ( this->type <  3 ) return 1;
	if ( this->type == 4 ) return this->pipe->size;
	return 0;
}

/*----------------------------------------------------------------------------*/



/*============================================================================*/


struct Ctx {
	UnitRef unit;

	Ctx(){
	}
	Ctx(Unit& unit){
	}
	Ctx(const Ctx& ctx){
	}
	~Ctx(){
	}

};


struct CtxRandom : Ctx {
	Pipe    idx;

	CtxRandom& operator[](uint64_t idx){this->idx.put(idx);return *this;}
	CtxRandom& operator[](String   idx){this->idx.put(idx);return *this;}

	String toStr(){return this->idx.toStr();}
};


/*----------------------------------------------------------------------------*/





/*============================================================================*/

struct UnitStream : Unit {
	void    node_stat(CtxRandom& ctx){}
	size_t  node_size(CtxRandom& ctx){}
	void    node_clear(CtxRandom& ctx){}

	size_t  node_get_raw(CtxRandom& ctx, char* dst, size_t size){}
	Num     node_get_num(CtxRandom& ctx, char type){}
	void    node_get_str(CtxRandom& ctx, String& out){}
	UnitRef node_get_col(CtxRandom& ctx){}

	size_t  node_put_raw(CtxRandom& ctx, const char* dst, size_t size){}
	void    node_put_num(CtxRandom& ctx, char type, Num val){}
	void    node_put_str(CtxRandom& ctx, String val){}
	void    node_put_col(CtxRandom& ctx, Ctx& src){}

	UnitStream& put(int    val){this->root_put_num(1,Num(val));return *this;}
	UnitStream& put(String val){this->root_put_str(val);return *this;}

	void endl(){this->root_end_line();}
};

struct UnitRandom : Unit {
	CtxRandom operator[](uint64_t idx){}
	CtxRandom operator[](String   idx){}
};

struct UnitVector : UnitRandom {
};

struct UnitMap : UnitRandom {
};

/*----------------------------------------------------------------------------*/
