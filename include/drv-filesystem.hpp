#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>



struct FileSystem : Map {
	String root;

	FileSystem(String root="./"){
		ctx.api.stat     = ctx_stat;
		ctx.api.to_str   = ctx_to_str;
		ctx.api.set_str  = ctx_set_str;
		ctx.api.seek_str = ctx_seek_str;
		ctx.api.mkmap    = ctx_mkmap;
		this->root = root;
	}

	Cursor begin(){
	}


	// virtuals functions
	size_t size(){}

	static Stat ctx_stat(Ctx* ctx){
		struct stat buf;
		if ( ::stat(ctx->idx_s.c_str(), &buf) == 0 ){
			if ( S_ISREG(buf.st_mode) ){
				return Stat().setItem();
			} else {
				return Stat().setMap();
			}
		}
		return Stat(0);
	}

	static void ctx_set_str(Ctx* ctx, String val){
		if ( ctx->stat().isItem() || ctx->stat().isNull() ){
			FILE* fd = fopen(ctx->idx_s.c_str(), "w");
			if ( fd ){
				fwrite(val.c_str(), val.size(), 1, fd);
				fclose(fd);
			}
		}
	}

	static String ctx_to_str(Ctx* ctx){
		String res;
		if ( ctx->stat().isItem() ){
			char buf[4096];
			FILE* fd = fopen(ctx->idx_s.c_str(), "r");
			if ( !fd ){
				return res;
			}
			do {
				fgets(buf,4096,fd);
				res += buf;
			} while ( !feof(fd) );
			fclose(fd);
		} else if ( ctx->stat().isColl() ){

		}
		return res;
	}


	static Ctx ctx_mkmap(Ctx* ctx){
		if( ctx->isItem() ){
			cout << "error!\n";
			return Ctx();
		}
		if ( ctx->isNull() ){
			::mkdir(ctx->idx_s.c_str(),0755);
		}
		FileSystem* fs = new FileSystem(ctx->idx_s);
		Ctx res;
		res = fs->ctx;
		res.coll = fs;
printf("aaa %p\n",res.coll);
		return res;
	}

	static void ctx_seek_str(Ctx* ctx, String idx){
		FileSystem* self = (FileSystem*) ctx->coll;
		ctx->idx_s = self->joinPath(idx);
	}


	void prepareRoot(){
		size_t size = this->root.size();
		while ( size>0 && this->root[size-1] == '/' ){
			this->root.resize(size-1);
			size = this->root.size();
		}
	}

	String joinPath(String p2){
		size_t size = this->root.size();
		if ( size > 0 ){
			return this->root + "/" + p2;
		} else {
			return p2;
		}
	}

};
