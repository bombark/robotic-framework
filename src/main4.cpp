#include "core.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <pthread.h>

using namespace std;



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

	void     stream_stat   (CtxStream* ctx){}
	size_t   stream_get_raw (CtxStream* ctx, void* dst, size_t size){
		return fread(dst,1,size,fd);
	}
	size_t   stream_put_raw (CtxStream* ctx, const void* src, size_t size){
		return fwrite(src,1,size,fd);
	}

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
};


struct FsItemRandom : FsItemStream {
	FsItemRandom(){this->fd = NULL;}
	FsItemRandom(const char* name, const char* mode) : FsItemStream(name,mode){}
	FsItemRandom(FILE* fd) : FsItemStream(fd){}


	void random_open_stream (CtxRandom* ctx, CtxStream& dst, char mode){
		dst.idx = 0;
	}

	void random_open_random (CtxRandom* ctx, CtxRandom& dst, char mode){
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

	void random_put_nat  (CtxRandom* ctx, uint64_t val){}
	void random_put_int  (CtxRandom* ctx, int64_t  val){}
	void random_put_flt  (CtxRandom* ctx, double   val){}
	void random_put_str  (CtxRandom* ctx, String   val){}

	Stat random_stat   (CtxRandom* ctx){Stat(0);}

	void random_erase  (CtxRandom* ctx, bool index){}

	void random_resize (CtxRandom* ctx, size_t size){}
	uint64_t random_size   (CtxRandom* ctx){return 1;}


	uint64_t rawsize(){return 1;}

	uint64_t size(){return 1;}



	void seek(CtxRandom* ctx){
		size_t pos = ctx->idx.getNat(0);
		fseek(fd,pos,SEEK_SET);
	}

	String toStr(){
		return "";
	}
};






struct VetBufferRow : UnitVector {
	String format;
	std::vector<String> data;

	VetBufferRow(){
		data.resize(10);
	}

	void openRandom (CtxRandom& ctx){
		ctx.idx  = 0;
	}

	void openStream (CtxStream& ctx){
		ctx.idx  = 0;
	}

	void get_raw(Ctx* ctx, void* src, size_t size){}

	void get_nat(Ctx* ctx, uint64_t& dst){
		size_t idx = ctx->idx.getNat(0);
		dst = atoi(data[idx].c_str());
	}

	void get_int(Ctx* ctx, int64_t&  dst){
		size_t idx = ctx->idx.getNat(0);
		dst = atoi(data[idx].c_str());
	}

	void get_flt(Ctx* ctx, double&   dst){
		size_t idx = ctx->idx.getNat(0);
		dst = atof(data[idx].c_str());
	}

	void get_str(Ctx* ctx, String&   dst){
		size_t idx = ctx->idx.getNat(0);
		dst = data[idx];
	}

	void put_raw(Ctx* ctx, const void* src, size_t size){}

	void put_nat(Ctx* ctx, uint64_t val){
		size_t idx = ctx->idx.getNat(0);
		char buf[64]; sprintf(buf,"%lu",val);
		data[idx] = buf;
		format[idx] = 'n';
	}

	void put_int(Ctx* ctx, int64_t  val){
		size_t idx = ctx->idx.getNat(0);
		char buf[64]; sprintf(buf,"%ld",val);
		data[idx] = buf;
		format[idx] = 'd';
	}

	void put_flt(Ctx* ctx, double   val){
		size_t idx = ctx->idx.getNat(0);
		char buf[64]; sprintf(buf,"%lf",val);
		data[idx]   = buf;
		format[idx] = 'f';
	}

	void put_str(Ctx* ctx, String   val){
		size_t idx  = ctx->idx.getNat(0);
		data[idx]   = val;
		format[idx] = 's';
	}

	void put_cmd(Ctx* ctx, int type){}
	uint64_t size(){return data.size();}

	String toStr(){
		String res;
		for ( int i=0; i<data.size(); i++){
			res += data[i];
			res += ';';
		}
		return res;
	}

};




struct ItemString : Unit {
	String data;

	ItemString(String data){this->data = data;}

	void get_raw(Ctx* ctx, void* _dst, size_t size){
		size_t idx = ctx->idx.getNat(0);
		size_t rest = this->data.size() - idx;
		if ( rest < size )
			size = rest;
		char* dst  = (char*) _dst;
		for (size_t i=0; i<size; ++i,++dst){
			*dst = data[idx+i];
		}
		if ( ctx->isStream() ){
			ctx->idx.atNat(0) += size;
		}
	}

	void put_raw(Ctx* ctx, const void* src, size_t size){}

	uint64_t size(){return data.size();}


	void random_open_random (CtxRandom* ctx, CtxRandom& dst, char mode){}
	void random_open_stream (CtxRandom* ctx, CtxStream& dst, char mode){
		dst.idx = 0;
	}
	void get_nat(Ctx* ctx, uint64_t& dst){dst = atoi(data.c_str());}
	void get_int(Ctx* ctx, int64_t&  dst){dst = atoi(data.c_str());}
	void get_flt(Ctx* ctx, double&   dst){dst = atof(data.c_str());}
	void get_str(Ctx* ctx, String&   dst){dst = data;}

	void put_nat(Ctx* ctx, uint64_t val){
		char buf[64]; sprintf(buf,"%lu",val);
		data = buf;
	}
	void put_int(Ctx* ctx, int64_t  val){
		char buf[64]; sprintf(buf,"%ld",val);
		data = buf;
	}
	void put_flt(Ctx* ctx, double   val){
		char buf[64]; sprintf(buf,"%lf",val);
		data = buf;
	}
	void put_str(Ctx* ctx, String   val){
		data = val;
	}

	void put_cmd(Ctx* ctx, int type){}
	String toStr(){return data;}
};






int main(int argc, char** argv){
	FsItemStream file1(stdout);

	FsItemRandom file2("data.txt","w+");

	Raw raw = file2;
	raw[0].put8("felipe bombardelli\n");
	raw[10].put32(0x41424344);




	/*ItemString tmp("felipe bombardelli");

	ReaderRaw reader = tmp;

	cout << reader.get() << endl;
	cout << reader.get() << endl;
	cout << reader.get() << endl;*/

	/*Raw name = tmp;
	name(0,4).blank();
	name(0,6).erase();
	name[2].put("teste");
	name[2].insert("teste");
	cout << name.toStr() << endl;*/

	/*Val val = tmp;
	val = "teste"; cout << val.toStr() << endl;
	val = 10;      cout << val.toStr() << endl;
	val = 23.4123; cout << val.toStr() << endl;*/




	/*VetBufferRow row;
	VetPtr vet(row);

	vet[0] = "string";
	vet[1] = 10;
	vet[2] = 10.44;


	vet.put("fim");

	cout << vet[3].toStr() << endl;

	cout << vet.size() << endl;
	cout << vet[2].stat() << endl;*/



	//FsFile pfile(stdout);
	//WriterVal raw(pfile);
	//raw.put("score:").put(10).endl();

	/*ReaderVet vet(pfile);
	while(1){
		vet.next();
		cout << vet[0].toStr();
	}*/

	//ReaderVal reader(pfile);
	//reader.get()

	//Teste teste;


	//cout << reader.toStr() << endl;

	//RosCore ros("pub1");

	/*if ( argc == 1 ){
		RosCore ros("pub1");

		RosPose ppose(node,"chatter",'w');
		Stream pose = ppose;
		for(int i=0; i<10; i++){
			pose.put(30+i).put("66").put(20).put(33.333).put(40).put(50).put(60).endl();
			sleep(1);
		}

	} else {
		RosCore ros("sub1");
		FsFile pfile(stdout);
		TextDrv ptext(pfile);
		Stream out = ptext;
		RosPose ppose(node,"chatter",out);
		StreamMsg pose = ppose;
		for(int i=0; i<4; i++){
			pose.read();
			//cout << pose.getNat() << endl;
			sleep(1);
		}
	}*/


	//FsFile pfile(stdout);//("data.txt","w");
	//TextDrv ptext(pfile);
	//CSV pcsv(pfile,"w");

	/*Stream csv = pcsv;
	for ( int i=0; i<10; i++){
		csv.put(i).put(20).put(30).put("felipe").endl();
	}*/


	/*Stream a = file;
	a.putRawAs<short>(10);
	a.putRawAs<short>(20);
	a.putRawAs<int>(0x10000000);
	int v1 = a.getRawAs<short>();
	int v2 = a.getRawAs<short>();
	int v3 = a.getRawAs<char>();
	cout << v1 << " " << v2 << " " << v3 << endl;*/

	//TextDrv text(file);


	/*Random rnd(file);
	rnd[0].put(130);
	rnd[4].put(20);
	rnd[8].put(30);*/

	/*int a = rnd[0].getInt8();
	int b = rnd[4].getInt8();
	int c = rnd[8].getInt32();
	cout << a << " " << b << endl;*/

	/*Stream out(text);
	out.put(10).put(20).put("fel\nipe\n bombar\ndelli").endl();
	out.put(10).put(22).put(32).endl();
	out.put(10).put(42).put(12).endl();
	//out.put(20).mkvet().leave().put(30).endl();
	//out.mkmap().put("name","felipe").put("idade",20).leave().endl();
	out.put(20).format("{name: ?,score: ?}").put("felipe").put(10).leave().put(30).endl();*/

	//cout << out.getIdx() << endl;*/
}
