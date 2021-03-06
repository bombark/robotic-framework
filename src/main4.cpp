#include "core.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <pthread.h>

using namespace std;



#include "drv-filesystem.cpp"
#include "drv-yaml.cpp"





void lista(MapPtr node, int level=0){
	//Reader reader=node;
	//while ( reader.read() ){
		/*Stat stat_row = reader.statRow();
		if ( stat_row.isItem() ){
			for (int i=0;i<level;i++)
				cout << "  ";
			cout << reader.idx.getStr(0) << ": \n";
		} else if ( stat_row.isVet() ){
			for (int i=0;i<level;i++)
				cout << "  ";
			cout << reader.idx.getStr(0) << endl;
		} else if ( stat_row.isMap() ){
			for (int i=0;i<level;i++)
				cout << "  ";
			String idx = reader.idx.getStr(0);
			cout << "a " << idx << endl;
			//if ( level >= 1 ) break;
			lista( node[idx], level+1 );
		}*/
	//}
}



/*struct FsItemStream : StreamData {
	size_t raw_get (CtxStream& ctx, char* dst, size_t size){
	}
	size_t raw_put (CtxStream& ctx, char* dst, size_t size){
	}
	void  base_update(CtxStream& ctx){
	}
	uint64_t  val_get_nat (CtxStream& ctx){
	}
	int64_t   val_get_int (CtxStream& ctx){
	}
	double    val_get_flt (CtxStream& ctx){
	}
	String    val_get_str (CtxStream& ctx){
	}
	CtxRandom val_get_vet (CtxStream& ctx){}
};*/



std::vector<string> explode(String text, char div){
	String buf;
	std::vector<string> res;
	size_t size=text.size();
	for ( size_t i=0; i<size; i++){
		char c = text[i];
		if ( c == div ){
			if ( buf.size() > 0 ){
				res.push_back(buf);
				buf.clear();
			}
		} else {
			if ( buf.size() == 0 && c == ' ' ) continue;
			buf += c;
		}
	}
	if ( buf.size() > 0 ){
		res.push_back(buf);
	}
	return res;
}



struct MapLine2Stream : UnitVector {
	VetPtr data;
	CtxRandom row;
	std::vector<string> header;
	size_t row_id;

	MapLine2Stream(VetPtr& vet, String format){
		this->data = vet;
		header = explode(format,',');
		row_id = 0;
		row = this->data[0].openAsMap();
	}

	uint64_t size(){return header.size();}
	uint64_t toNat(){}
	int64_t  toInt(){}
	double   toFlt(){}
	String   toStr(){}
	void     setNat(uint64_t val){}
	void     setInt(int64_t  val){}
	void     setFlt(double   val){}
	void     setStr(String   val){}
	void     random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){}
	void     root_open_stream (CtxStream& dst, char mode){}
	void     node_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode){}

	Stat     random_stat   (CtxRandom* ctx){
		size_t idx = ctx->idx.getNat(0);
		return row[ header[idx] ].stat();
	}

	void     random_erase  (CtxRandom* ctx, bool   index){}

	void     random_resize (CtxRandom* ctx, size_t size){}

	uint64_t random_size   (CtxRandom* ctx){
		//size_t idx = ctx->idx.getNat(0);
		//return vet[ header[idx] ].size();
	}

	uint64_t random_get_raw (CtxRandom* ctx, void* dst, size_t size){}
	uint64_t random_put_raw (CtxRandom* ctx, const void* src, size_t size){}
	uint64_t random_get_nat (CtxRandom* ctx){
		size_t idx = ctx->idx.getNat(0);
		return row[ header[idx] ].toNat();
	}
	int64_t  random_get_int (CtxRandom* ctx){
		size_t idx = ctx->idx.getNat(0);
		return row[ header[idx] ].toInt();
	}
	double   random_get_flt (CtxRandom* ctx){
		size_t idx = ctx->idx.getNat(0);
		return row[ header[idx] ].toFlt();
	}
	void     random_get_str (CtxRandom* ctx, String& dst){
		size_t idx = ctx->idx.getNat(0);
		dst += row[ header[idx] ].toStr();
	}
	void     random_put_nat  (CtxRandom* ctx, uint64_t val){}
	void     random_put_int  (CtxRandom* ctx, int64_t  val){}
	void     random_put_flt  (CtxRandom* ctx, double   val){}
	void     random_put_str  (CtxRandom* ctx, String   val){}
	void     random_make      (CtxRandom* ctx, Stat   type){}
	void     random_make_item (CtxRandom* ctx, size_t size){}
};


int main(int argc, char** argv){
	FsItem file("data.txt","r");

	YamlNode data("{name: felipe, score: 10}");

	Iterator  it = data.begin();
	CtxRandom row = it.read();

	Stat stat = row.stat();
	cout << row.base.getStr(0) << endl;
	if ( stat.isItem() ){
		cout << "item\n";
	} else {
		cout << "coll\n";
	}

	/*FsNode name("pessoa/name");
	name.mkitem();
	name.resize(200);
	Reader raw = name.toReader();*/




	/*cout << raw.getc() << endl;
	cout << raw.getc() << endl;*/

	/*YamlNode data( file.toStr() );
	VetPtr base = data["data"].openAsVet();
	MapLine2Stream a(base,"z,y,x");
	cout << a[1].toStr() << endl;*/

	/*ReaderVet vet2(base);
	while ( vet.read() ){
	}*/



	//cout << data["data"][1].toStr() << endl;

	/*

	data.log();
	CtxRandom ctx = data;
	ctx.base.putStr("data");
	ctx["name"].log();*/
	//cout << ctx["name"].stat() << endl;

	//cout << data["data"].stat() << endl;

	/*VetPtr vet = data["data"];

	Iterator it = data.begin();
	for (; it.isOk(); ++it ){
		Stat stat = it.stat();
		if ( stat.isItem() ){
			cout << it.key() << ": " << it.toStr() << endl;
		} else if ( stat.isVet() ){
			cout << it.key() << ": \n";
			VetPtr vet = it.toVet();
			vet.log();
			//cout << vet.size() << " " << vet[0].toStr() << endl;
		} else if ( stat.isMap() ){
			cout << it.key() << ":n \n";
		}
	}*/


	/*file.log();
	data.log();
	cout << data["datdddda"][0].stat() << endl;
	cout << data["vetor"][0].stat() << endl;
	cout << data["data"].stat() << endl;
	cout << data["data"][0].stat() << endl;
	cout << data["data"][0]["score"].stat() << endl;*/
	//cout <<  << endl;

	//CtxRandom a = vet[0];
	//cout << a.toStr() << endl;


	//lista(fs,0);


	//map["data"]["teste"].mkitem(1024);
	//Raw raw = map["data"]["teste"].open();

	//MapPtr data = map["data"];
	//cout << map["data/teste"].toStr() << endl;

	//Raw raw = map["data/teste"].open();
	//raw[0].put8("felipe gustavo bombardelli\n");



	/*FsItemStream file1(stdout);
	FsItemRandom file2("data.txt","w+");
	Raw raw = file2;
	raw[10].put32(0x41424344);*/




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





struct VetBufferRow : UnitVector {
	String format;
	std::vector<String> data;

	VetBufferRow(){
		data.resize(10);
	}

	void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){
		dst.idx  = 0;
	}

	void get_raw(CtxRandom* ctx, void* src, size_t size){}

	uint64_t random_get_nat(CtxRandom* ctx){
		size_t idx = ctx->idx.getNat(0);
		return atoi(data[idx].c_str());
	}

	int64_t random_get_int(CtxRandom* ctx){
		size_t idx = ctx->idx.getNat(0);
		return atoi(data[idx].c_str());
	}

	double random_get_flt(CtxRandom* ctx){
		size_t idx = ctx->idx.getNat(0);
		return atof(data[idx].c_str());
	}

	void random_get_str(CtxRandom* ctx, String&   dst){
		size_t idx = ctx->idx.getNat(0);
		dst += data[idx];
	}

	uint64_t random_put_raw(CtxRandom* ctx, const void* src, size_t size){}

	void random_put_nat(CtxRandom* ctx, uint64_t val){
		size_t idx = ctx->idx.getNat(0);
		char buf[64]; sprintf(buf,"%lu",val);
		data[idx] = buf;
		format[idx] = 'n';
	}

	void random_put_int(CtxRandom* ctx, int64_t  val){
		size_t idx = ctx->idx.getNat(0);
		char buf[64]; sprintf(buf,"%ld",val);
		data[idx] = buf;
		format[idx] = 'd';
	}

	void random_put_flt(CtxRandom* ctx, double   val){
		size_t idx = ctx->idx.getNat(0);
		char buf[64]; sprintf(buf,"%lf",val);
		data[idx]   = buf;
		format[idx] = 'f';
	}

	void random_put_str(CtxRandom* ctx, String   val){
		size_t idx  = ctx->idx.getNat(0);
		data[idx]   = val;
		format[idx] = 's';
	}

	void random_put_cmd(CtxRandom* ctx, int type){}
	uint64_t size(){return data.size();}


	uint64_t random_get_raw (CtxRandom* ctx, void* dst, size_t size){}
	Stat random_stat   (CtxRandom* ctx){}
	void random_erase  (CtxRandom* ctx, bool   index){}
	void random_resize (CtxRandom* ctx, size_t size){}
	uint64_t random_size   (CtxRandom* ctx){}
	void stream_save      (CtxStream* ctx, uint8_t level){}
	void stream_load_line (CtxStream* ctx){}
	uint64_t rawsize(){}


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
		/*if ( ctx->isStream() ){
			ctx->idx.atNat(0) += size;
		}*/
	}

	void put_raw(Ctx* ctx, const void* src, size_t size){}

	uint64_t size(){return data.size();}


	void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){}
	void random_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode){
		dst.idx = 0;
	}
	void get_nat(Ctx* ctx, uint64_t& dst){dst = atoi(data.c_str());}
	void get_int(Ctx* ctx, int64_t&  dst){dst = atoi(data.c_str());}
	void get_flt(Ctx* ctx, double&   dst){dst = atof(data.c_str());}
	void get_str(Ctx* ctx, String&   dst){dst = data;}

	void random_put_nat(CtxRandom* ctx, uint64_t val){
		char buf[64]; sprintf(buf,"%lu",val);
		data = buf;
	}
	void random_put_int(CtxRandom* ctx, int64_t  val){
		char buf[64]; sprintf(buf,"%ld",val);
		data = buf;
	}
	void random_put_flt(CtxRandom* ctx, double   val){
		char buf[64]; sprintf(buf,"%lf",val);
		data = buf;
	}
	void random_put_str(CtxRandom* ctx, String   val){
		data = val;
	}

	void put_cmd(Ctx* ctx, int type){}
	String toStr(){return data;}

	size_t   random_get_raw (CtxRandom* ctx, void* dst, size_t size){}
	size_t   random_put_raw (CtxRandom* ctx, const void* src, size_t size){}
	void     random_get_str (CtxRandom* ctx, String& dst){}

	Stat random_stat   (CtxRandom* ctx){}
	void random_erase  (CtxRandom* ctx, bool   index){}
	void random_resize (CtxRandom* ctx, size_t size){}
	uint64_t random_size   (CtxRandom* ctx){}
	void stream_save      (CtxStream* ctx, uint8_t level){}
	void stream_load_line (CtxStream* ctx){}
	uint64_t rawsize(){}

};
