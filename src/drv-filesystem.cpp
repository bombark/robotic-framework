/*============================================================================*/

#include "core.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

/*----------------------------------------------------------------------------*/

/*
struct FsItemStream : UnitStream {
	FILE* fd;

	FsItemStream(){
		this->fd = stdout;
	}

	FsItemStream(const char* name, const char* mode){
		this->fd = fopen(name,mode);
	}

	FsItemStream(FILE* fd){
		this->fd = fd;
	}

	Stat     stream_stat_line   (CtxStream* ctx){}
	size_t   stream_get_raw (CtxStream* ctx, void* dst, size_t size){
		return fread(dst,1,size,fd);
	}
	size_t   stream_put_raw (CtxStream* ctx, const void* src, size_t size){
		return fwrite(src,1,size,fd);
	}

	void      stream_save_line (CtxStream* ctx, uint8_t level){}
	CtxRandom stream_get_line  (CtxStream* ctx){}

	uint64_t stream_get_nat (CtxStream* ctx){return 0;}
	int64_t  stream_get_int (CtxStream* ctx){return 0;}
	double   stream_get_flt (CtxStream* ctx){return 0;}
	void     stream_get_str (CtxStream* ctx, String& dst){}

	void     stream_put_nat (CtxStream* ctx, uint64_t val){
		fwrite(&val,1,sizeof(val),fd);
	}
	void     stream_put_int (CtxStream* ctx, int64_t  val){
		fwrite(&val,1,sizeof(val),fd);
	}
	void     stream_put_flt (CtxStream* ctx, double   val){
		fwrite(&val,1,sizeof(val),fd);
	}
	void     stream_put_str (CtxStream* ctx, String   val){
		fwrite(val.c_str(),1,val.size(),fd);
	}

	void stream_save      (CtxStream* ctx, uint8_t level){}
	void stream_load_line (CtxStream* ctx){}
};*/

/*============================================================================*/

struct FsItem : UnitItem {
	FILE* fd;

	FsItem() : UnitItem() {this->fd = NULL;}
	FsItem(const char* name, const char* mode) : UnitItem(){
		this->fd = fopen(name,mode);
	}
	FsItem(FILE* fd) : UnitItem(){}

	~FsItem(){
		if ( fd ){ fclose(fd); }
	}

	String classe(){return "Fs/File";}

	uint64_t toNat(){}
	int64_t  toInt(){}
	double   toFlt(){}
	void     setNat(uint64_t val){}
	void     setInt(int64_t  val){}
	void     setFlt(double   val){}
	void     setStr(String   val){}



	void root_open_stream (CtxStream& dst, char mode){
		dst.base = 0;
		dst.raw.read = stx_raw_read;
		dst.raw.write = stx_raw_write;
		dst.raw.is_ok = stx_raw_is_ok;
	}

	void root_open_random (CtxRandom& dst, char mode){}

	void node_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode){
		dst.idx = 0;
	}

	void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){
		dst.idx = 0;
	}

	size_t   random_get_raw (CtxRandom* ctx, void* dst, size_t size){
		this->seek(ctx);
		return fread(dst,1,size,this->fd);
	}

	size_t   random_put_raw (CtxRandom* ctx, const void* src, size_t size){
		this->seek(ctx);
		return fwrite(src,1,size,this->fd);
	}

	uint64_t random_get_nat (CtxRandom* ctx){return 0;}
	int64_t  random_get_int (CtxRandom* ctx){return 0;}
	double   random_get_flt (CtxRandom* ctx){return 0;}
	void     random_get_str (CtxRandom* ctx, String& dst){}

	void random_put_nat  (CtxRandom* ctx, uint64_t val){
		size_t size = ctx->idx.getNat(0);
		fseek(fd,size,SEEK_SET);
		fprintf(fd,"%lu",val);
	}
	void random_put_int  (CtxRandom* ctx, int64_t  val){
		size_t size = ctx->idx.getNat(0);
		fseek(fd,size,SEEK_SET);
		fprintf(fd,"%ld",val);
	}
	void random_put_flt  (CtxRandom* ctx, double   val){
		size_t size = ctx->idx.getNat(0);
		fseek(fd,size,SEEK_SET);
		fprintf(fd,"%lf",val);
	}
	void random_put_str  (CtxRandom* ctx, String   val){
		size_t idx = ctx->idx.getNat(0);
		fseek(fd,idx,SEEK_SET);
		fprintf(fd,"%s",val.c_str());
	}

	Stat random_stat   (CtxRandom* ctx){return Stat().setByte();}

	void random_erase  (CtxRandom* ctx, bool index){}

	void random_resize (CtxRandom* ctx, size_t size){}
	uint64_t random_size   (CtxRandom* ctx){return 1;}


	uint64_t rawsize(){return 1;}

	uint64_t size(){return 1;}

	void     random_make      (CtxRandom* ctx, Stat   type){}
	void     random_make_item (CtxRandom* ctx, size_t size){}

	void seek(CtxRandom* ctx){
		size_t pos = ctx->idx.getNat(0);
		fseek(fd,pos,SEEK_SET);
	}

	String toStr(){
		String res;
		char buffer[4096];
		fseek(fd,0,SEEK_SET);
		while ( fgets(buffer,4096,fd) ){
			res += buffer;
		}
		return res;
	}

	static void stx_raw_read(CtxStream* ctx, char* dst, size_t size){
		FsItem* self = (FsItem*) ctx->unit;
		fread( dst, 1, size, self->fd );
	}

	static void stx_raw_write(CtxStream* ctx, const char* src, size_t size){
		FsItem* self = (FsItem*) ctx->unit;
		fwrite( src, 1, size, self->fd );
	}

	static bool stx_raw_is_ok(CtxStream* ctx){
		FsItem* self = (FsItem*) ctx->unit;
		return !feof(self->fd);
	}
};



struct FsItemStream : StreamData {
	size_t raw_get (CtxStream& ctx, char* dst, size_t size){
		FsItem* unit = (FsItem*) ctx.unit;
		return fread(dst,1,size,unit->fd);
	}
	size_t raw_put (CtxStream& ctx, char* dst, size_t size){
		FsItem* unit = (FsItem*) ctx.unit;
		return fwrite(dst,1,size,unit->fd);
	}
	void  base_update(CtxStream& ctx){
		FsItem* unit = (FsItem*) ctx.unit;
		ctx.base = ftell(unit->fd);
	}
	uint64_t  val_get_nat (CtxStream& ctx){
		FsItem* unit = (FsItem*) ctx.unit;
		uint64_t res; fscanf(unit->fd,"%lu",&res);
		return res;
	}
	int64_t   val_get_int (CtxStream& ctx){
		FsItem* unit = (FsItem*) ctx.unit;
		int64_t res; fscanf(unit->fd,"%ld",&res);
		return res;
	}
	double    val_get_flt (CtxStream& ctx){
		FsItem* unit = (FsItem*) ctx.unit;
		double res; fscanf(unit->fd,"%lf",&res);
		return res;
	}
	String    val_get_str (CtxStream& ctx){
		FsItem* unit = (FsItem*) ctx.unit;
		char buffer[4096];
		fscanf(unit->fd,"%4096s",buffer);
		return buffer;
	}
	CtxRandom val_get_vet (CtxStream& ctx){}
};




/*struct StreamDataText : StreamData {
	StreamData* raw;

	size_t raw_get (CtxStream& ctx, char* dst, size_t size){
		return this->raw->raw_get(*this,dst,size);
	}

	size_t raw_put (CtxStream& ctx, char* dst, size_t size){
		return this->raw->raw_get(*this,dst,size);
	}

	uint64_t  val_get_nat (CtxStream& ctx){
		char c = this->getc();
	}

	int64_t   val_get_int (CtxStream& ctx){}
	double    val_get_flt (CtxStream& ctx){}
	String    val_get_str (CtxStream& ctx){}
	CtxRandom val_get_vet (CtxStream& ctx){}

	char getc(){
		char c; this->raw->raw_get(*this,&c,1);
		return c;
	}
};*/


/*----------------------------------------------------------------------------*/







/*============================================================================*/

struct FsDirStreamData : StreamData {
	DIR*   dir;
	bool   is_ok;

	FsDirStreamData(String url){
		this->dir = opendir (url.c_str());
		this->is_ok = ( this->dir ) ? true : false;
	}

	Stat stat(){return Stat(0);}

	void first(CtxStream& ctx){
		this->next(ctx);
	}

	void prev(CtxStream& ctx){}

	void next(CtxStream& ctx){
		if ( this->dir == NULL ) return;
		String name;
		do {
			struct dirent *entry = readdir(this->dir);
			if ( !entry ){
				this->is_ok = false;
				return;
			}
			name = entry->d_name;
		} while ( name == "." || name == ".." );
		ctx.idx = name;
	}

	bool isOk(){
		return this->is_ok;
	}

	CtxRandom val_get_vet (CtxStream& ctx){
		return CtxRandom();
	}
};

/*----------------------------------------------------------------------------*/


/*============================================================================*/

struct FsSystem : UnitTree {
	String root;

	FsSystem(String root){
		this->root  = root;
		size_t size = this->root.size();
		if ( size>1 ){
			size_t i=size-1;
			if ( this->root[i] == '/' )
				this->root.resize(size-1);
		}
	}

	Stat      stat(){return Stat().setMap();}
	uint64_t  size(){return 0;}

	void root_open_stream (CtxStream& dst, char mode){}

	void node_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode){
		String url = this->ctx2url((CtxRandom*)ctx);
//LOG(url.c_str());
		Stat node_stat = this->getStat(url);
		if ( node_stat.isColl() ){
			dst.stream_data = new FsDirStreamData(url);
		} else {
			dst.stream_data = NULL;
		}
	}


	Stat      stream_stat_line (CtxStream* ctx){}
	void      stream_save_line (CtxStream* ctx, uint8_t level){}
	CtxRandom stream_get_line  (CtxStream* ctx){}

	size_t   stream_get_raw (CtxStream* ctx, void* dst, size_t size){}
	size_t   stream_put_raw (CtxStream* ctx, const void* src, size_t size){}
	uint64_t stream_get_nat (CtxStream* ctx){}
	int64_t  stream_get_int (CtxStream* ctx){}
	double   stream_get_flt (CtxStream* ctx){}
	void     stream_get_str (CtxStream* ctx, String& dst){}
	void     stream_put_nat (CtxStream* ctx, uint64_t val){}
	void     stream_put_int (CtxStream* ctx, int64_t  val){}
	void     stream_put_flt (CtxStream* ctx, double   val){}
	void     stream_put_str (CtxStream* ctx, String   val){}
	void     stream_save      (CtxStream* ctx, uint8_t level){}
	void     stream_load_line (CtxStream* ctx){}

	Stat     random_stat   (CtxRandom* ctx){
		String url = this->ctx2url(ctx);
		return this->getStat(url);
	}


	void     random_erase  (CtxRandom* ctx, bool   index){}
	void     random_resize (CtxRandom* ctx, size_t size){}
	uint64_t random_size   (CtxRandom* ctx){}



	void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){
		String url = this->ctx2url((CtxRandom*)ctx);
		Stat node_stat = this->getStat(url);
		if ( node_stat.isItem() ){
			dst.unit = new FsItem(url.c_str(),"r+");
		} else if ( node_stat.isColl() ){
			dst.unit = new FsSystem(url);
			/*dst.unit = this;
			dst.idx.clear();
			dst.idx.putStr(url);
			dst.idx.base = 1;*/
		}
	}

	uint64_t random_get_raw (CtxRandom* ctx, void* dst, size_t size){}
	uint64_t random_put_raw (CtxRandom* ctx, const void* src, size_t size){}

	uint64_t random_get_nat (CtxRandom* ctx){
		String url = this->ctx2url(ctx);
		Stat node_stat = this->getStat(url);
		if ( node_stat.isItem() ){
			FsItem file(url.c_str(),"r");
			return file.toNat();
		}
	}

	int64_t  random_get_int (CtxRandom* ctx){
		String url = this->ctx2url(ctx);
		Stat node_stat = this->getStat(url);
		if ( node_stat.isItem() ){
			FsItem file(url.c_str(),"r");
			return file.toInt();
		}
	}

	double   random_get_flt (CtxRandom* ctx){
		String url = this->ctx2url(ctx);
		Stat node_stat = this->getStat(url);
		if ( node_stat.isItem() ){
			FsItem file(url.c_str(),"r");
			return file.toFlt();
		}
	}

	void     random_get_str (CtxRandom* ctx, String& dst){
		String url = this->ctx2url(ctx);
		Stat node_stat = this->getStat(url);
		if ( node_stat.isItem() ){
			FsItem file(url.c_str(),"r");
			dst += file.toStr();
		}
	}

	void     random_put_nat (CtxRandom* ctx, uint64_t val){
	}
	void     random_put_int (CtxRandom* ctx, int64_t  val){}
	void     random_put_flt (CtxRandom* ctx, double   val){}
	void     random_put_str (CtxRandom* ctx, String   val){}

	void     random_make (CtxRandom* ctx, Stat type){
		if ( type.isItem() ){
			this->random_make_item(ctx,0);
		} else if ( type.isColl() ){
			String idx = this->ctx2url(ctx);
			if ( mkdir(idx.c_str(), 0755) != 0 ){
				ctx->err.putFatal( strerror(errno) );
			}
			if ( type.isVet() ){
			}
		}
	}

	void     random_make_item (CtxRandom* ctx, size_t size){
		String idx = this->ctx2url(ctx);
		FILE* fd = fopen(idx.c_str(), "w");
		if ( fd ){
			ftruncate(fileno(fd),size);
			fclose(fd);
		} else {
			ctx->err.putFatal( strerror(errno) );
		}
	}

protected:
	Stat   getStat(String url){
		struct stat path_stat;
		::stat(url.c_str(), &path_stat);
		if ( S_ISREG(path_stat.st_mode) ){
			return Stat().setItem();
		} else if ( S_ISDIR(path_stat.st_mode) ){
			return Stat().setMap();
		}
	}


	String ctx2url(CtxRandom* ctx){
		String url;
		url  = this->root;
		for (size_t i=0; i<ctx->idx.size; i++){
			url += '/';
			url += ctx->idx.getStr(i);
		}
		return url;
	}
};


/*Stat  FsDirStreamData::stat   (CtxStream* ctx){
	FsDirStreamData* buf = (FsDirStreamData*) ctx->data;
	if ( !buf->is_ok )
		return Stat(0);
	FsSystem fs(this->root);
	return fs.random_stat( (CtxRandom*)ctx );
	return Stat(0);
}*/

/*----------------------------------------------------------------------------*/
