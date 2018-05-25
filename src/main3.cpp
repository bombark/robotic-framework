/*============================================================================*/

#include "core2.hpp"
#include <iostream>
#include <stdarg.h>
#include <string.h>

using namespace std;

/*----------------------------------------------------------------------------*/


void c2e_put(Ctx* ctx, char type, ...){
	/*va_list args;
	va_start(args, type);
	if ( type == 0 ){
		ctx->c2e.put( va_arg(args, int) );
	} else if ( type == 1 ){
		ctx->c2e.put( va_arg(args, char*) );
	}*/
}


struct FsStream : UnitStream {
	FILE* fd;
	FsStream(){
		this->fd = stdout;
	}

	Stat    root_stat(){return Stat(0);}
	size_t  root_size(){return 0;}
	void    root_clear(){}

	size_t  root_get_raw(char* dst, size_t size){return fread(dst,1,size,fd);}

	Num     root_get_num(char type){
		Num res; fread(&res.nat64,1,sizeof(res),fd);
		return res;
	}

	void    root_get_str(String& out){}
	UnitRef root_get_col(){}
	size_t  root_put_raw(const char* dst, size_t size){fwrite(dst,1,size,fd);}
	void    root_put_num(char type, Num val){fwrite(&val.nat64,1,sizeof(val),fd);}
	void    root_put_str(String val){fwrite(val.c_str(),1,val.size(),fd);}
	void    root_put_col(Ctx& src){}

	UnitRef   root_mkitm(size_t size){}
	UnitStack root_mkvet_ini(){}
	UnitStack root_mkmap_ini(){}
	void      root_mkcol_end(Ctx& src){}

	void    root_end_line(){}
};



struct TextStream : UnitStream {
	Unit* base;
	char div_v;
	char div_l;

	TextStream(Unit& unit, char div_v=' ', char div_l='\n'){
		this->base = &unit;
		this->div_v = div_v;
		this->div_l = div_l;
	}

	Stat    root_stat(){return Stat(0);}
	size_t  root_size(){return 0;}
	void    root_clear(){}

	size_t  root_get_raw(char* dst, size_t size){
		return this->base->root_get_raw(dst,size);
	}

	Num     root_get_num(char type){

	}

	void    root_get_str(String& out){

	}

	UnitRef root_get_col(){}
	size_t  root_put_raw(const char* dst, size_t size){
		return this->base->root_put_raw(dst,size);
	}

	void    root_put_num(char type, Num val){
		char buf[64]; sprintf(buf, "%ld", val.int64);
		this->base->root_put_raw(buf,strlen(buf));
	}

	void    root_put_str(String val){
		this->base->root_put_raw(val.c_str(),val.size());
		this->base->root_put_raw(&div_v,1);
	}

	void    root_put_col(Ctx& src){}

	UnitRef root_mkitm(size_t size){}
	UnitRef root_mkvet_ini(){}
	UnitRef root_mkmap_ini(){}
	void    root_mkcol_end(Ctx& src){}

	void    root_end_line(){
		this->base->root_put_raw(&div_l,1);
	}
};



int main(){
	FsStream fs;
	TextStream text(fs,'|');
	//YamlStream text(fs,"x,y,z,w");

	text.put("aaa").put("ffff").put("eeee").put(20).endl();
	text.put("aaa").put("ffff").put("eeee").put(20).endl();
	text.put("aaa").put("ffff").put("eeee").put(20).endl();


}
