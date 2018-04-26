#include "core.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <pthread.h>

using namespace std;


struct FsFile : UnitBasic {
	FILE* fd;
	//struct stat buf_stat;

	FsFile(){
		this->fd = stdout;
	}

	FsFile(const char* name, const char* mode){
		this->fd = fopen(name,mode);
	}

	FsFile(FILE* fd){
		this->fd = fd;
	}

	void openRandom (Random& out){
	}

	void openStream (Stream& out){
	}


	uint64_t size(){}

	void get_raw  (Ctx* ctx, void* dst, size_t sizebytes){
		if ( ctx->isRandom() ){
			this->seek(ctx->idx.getNat(0));
		}
		size_t size = fread(dst,1,sizebytes,this->fd);
		if ( ctx->isStream() ){
			//Stream* stream = (Stream*) ctx;
			//stream->idx += size;
		}
	}

	void get_str(Ctx* ctx, String&   dst){}

	void put_raw  (Ctx* ctx, const void* src, size_t sizebytes){
		if ( ctx->isRandom() ){
			this->seek(ctx->idx.getNat(0));
		}
		size_t size = fwrite(src,1,sizebytes,this->fd);
		if ( ctx->isStream() ){
			//Stream* stream = (Stream*) ctx;
			//stream->idx += size;
		}
	}

	//void write_cmd (Ctx* ctx, String cmd){}

	void put_cmd(Ctx* stream, int type){}

	void seek(size_t pos){
		fseek(fd,pos,SEEK_SET);
	}

	String toStr(){
		return "";
	}
};




struct TextDrv : UnitBasic {
	Stream base;
	char endv,endl;
	bool first;

	TextDrv(Unit& unit, char endv=',', char endl='\n'){
		this->base.unit = &unit;
		this->endv = endv;
		this->endl = endl;
		this->first = true;
	}

	void openRandom (Random& pack){}
	void openStream (Stream& pack){}

	void get_raw (Ctx* ctx, void* dst, size_t size){
		base.getRaw(dst,size);
	}




	void put_raw (Ctx* ctx, const void* src, size_t size){
		base.putRaw(src,size);
	}

	void put_nat(Ctx* ctx, uint64_t val){
		this->putEndVar();
		char buf[64];
		sprintf(buf,"%lu",val);
		base.putRaw(buf,strlen(buf));
	}

	void put_int(Ctx* ctx, int64_t val){
		this->putEndVar();
		char buf[64];
		sprintf(buf,"%ld",val);
		base.putRaw(buf,strlen(buf));
	}

	void put_flt(Ctx* ctx, double val){
		this->putEndVar();
		char buf[64];
		sprintf(buf,"%lf",val);
		base.putRaw(buf,strlen(buf));
	}

	void put_str(Ctx* ctx, String str){
		this->putEndVar();
		String val;
		for (size_t i=0; i<str.size(); i++){
			char c = str[i];
			if ( c == endv ){
				val += '\\';
				val += endv;
			} else if ( c == endl ){
				val += '\\';
				val += 'n';
			} else {
				val += c;
			}
		}
		base.putStr(val);
	}



	String toStr(){}


	void put_cmd(Ctx* stream, int type){
		if ( type == 2 ){
			base.put(endl);
			this->first = true;
		}
	}


	void putEndVar(){
		if ( this->first ){
			this->first = false;
		} else {
			base.put(endv);
		}
	}

	void get_str(Ctx* ctx, String&   dst){}

	uint64_t size(){}


};





struct CSV : Unit {
	Stream stream;
	String line;
	vector<String> header;
	char endcol,endrow;
	bool is_first;

	CSV(Unit& unit, String mode, char endcol=',', char endrow='\n'){
		this->stream = unit;
		this->endcol = endcol;
		this->endrow = endrow;
		this->is_first = true;
		if ( mode == "r" ){
			this->readHeader();
		}
	}

	void openRandom (Random& pack){}
	void openStream (Stream& pack){}
	void get_raw(Ctx* ctx, void* src, size_t size){}
	void get_nat(Ctx* ctx, uint64_t& dst){}
	void get_int(Ctx* ctx, int64_t&  dst){}
	void get_flt(Ctx* ctx, double&   dst){}
	void get_str(Ctx* ctx, String&   dst){}

	void put_raw(Ctx* ctx, const void* src, size_t size){}

	void put_nat(Ctx* ctx, uint64_t val){
		this->putEndVar();
		this->stream.putNat(val);
	}
	void put_int(Ctx* ctx, int64_t  val){
		this->putEndVar();
		this->stream.putInt(val);
	}
	void put_flt(Ctx* ctx, double   val){
		this->putEndVar();
		this->stream.putInt(val);
	}
	void put_str(Ctx* ctx, String   str){
		this->putEndVar();
		this->stream.putStr(str);
	}

	void put_cmd(Ctx* ctx, int type){
		this->stream.putRawAs<char>(endrow);
		this->is_first = true;
	}

	uint64_t size(){}
	String toStr(){}


	void readHeader(){
		String buf;
		while ( true ){
			char c = stream.getRawAs<char>();
			if ( c == this->endrow ){
				if ( buf.size() > 0 ){
					header.push_back(buf);
					buf.clear();
				}
				break;
			} else if ( c == this->endcol ) {
				header.push_back(buf);
				buf.clear();
			} else {
				buf += c;
			}
		}
	}

	void readLine(){
		while ( true ){
			char c = stream.getRawAs<char>();
			if ( c == this->endrow ){
				break;
			} else if ( c == this->endcol ) {

			} else {
				line += c;
			}
		}
		cout << line << endl;
	}

	void putEndVar(){
		if ( this->is_first ){
			this->is_first = false;
		} else {
			//this->stream.putRawAs<char>(endcol);
		}
	}
};




/*
#include "ros/ros.h"
#include <geometry_msgs/Pose.h>



struct RosPose : Unit {
	String topic;
	ros::Publisher  pub;
	ros::Subscriber sub;
	geometry_msgs::Pose buf;
	pthread_mutex_t mutex;
	Stream event;


	RosPose(ros::NodeHandle& n, String topic, char mode){
		this->topic = topic;
		//if ( mode == 'w' ){
		this->pub = n.advertise<geometry_msgs::Pose>(topic, 1000);
		/*} else if ( mode == 'r' ){
			this->sub = n.subscribe(topic, 1000, &RosPose::callback, this);
			pthread_mutex_init(&this->mutex, NULL);
			pthread_mutex_lock(&this->mutex);
		}* /
	}

	RosPose(ros::NodeHandle& n, String topic, Stream& event){
		this->topic = topic;
		this->sub = n.subscribe(topic, 1000, &RosPose::callback, this);
		pthread_mutex_init(&this->mutex, NULL);
		pthread_mutex_lock(&this->mutex);
		this->event = event;
	}

	void openRandom (Random& ctx){}

	void openStream (Stream& ctx){
		ctx.idx.putNat(0);
	}

	void get_raw(Ctx* ctx, void* src, size_t size){}

	void get_nat(Ctx* ctx, uint64_t& dst){
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: dst = buf.position.x; break;
			case 1: dst = buf.position.y; break;
			case 2: dst = buf.position.z; break;
			case 3: dst = buf.orientation.x; break;
			case 4: dst = buf.orientation.y; break;
			case 5: dst = buf.orientation.z; break;
			case 6: dst = buf.orientation.w; break;
		}
	}

	void get_int(Ctx* ctx, int64_t&  dst){
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: dst = buf.position.x; break;
			case 1: dst = buf.position.y; break;
			case 2: dst = buf.position.z; break;
			case 3: dst = buf.orientation.x; break;
			case 4: dst = buf.orientation.y; break;
			case 5: dst = buf.orientation.z; break;
			case 6: dst = buf.orientation.w; break;
		}
	}

	void get_flt(Ctx* ctx, double&   dst){
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: dst = buf.position.x; break;
			case 1: dst = buf.position.y; break;
			case 2: dst = buf.position.z; break;
			case 3: dst = buf.orientation.x; break;
			case 4: dst = buf.orientation.y; break;
			case 5: dst = buf.orientation.z; break;
			case 6: dst = buf.orientation.w; break;
		}
	}

	void get_str(Ctx* ctx, String&   dst){
		double tmp;
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: tmp = buf.position.x; break;
			case 1: tmp = buf.position.y; break;
			case 2: tmp = buf.position.z; break;
			case 3: tmp = buf.orientation.x; break;
			case 4: tmp = buf.orientation.y; break;
			case 5: tmp = buf.orientation.z; break;
			case 6: tmp = buf.orientation.w; break;
		}
		char tmp_str[64]; sprintf(tmp_str,"%lf",tmp);
		dst += tmp_str;
	}

	void put_raw(Ctx* ctx, const void* src, size_t size){}

	void put_nat(Ctx* ctx, uint64_t val){
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: buf.position.x = val; break;
			case 1: buf.position.y = val; break;
			case 2: buf.position.z = val; break;
			case 3: buf.orientation.x = val; break;
			case 4: buf.orientation.y = val; break;
			case 5: buf.orientation.z = val; break;
			case 6: buf.orientation.w = val; break;
		}
	}

	void put_int(Ctx* ctx, int64_t  val){
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: buf.position.x = val; break;
			case 1: buf.position.y = val; break;
			case 2: buf.position.z = val; break;
			case 3: buf.orientation.x = val; break;
			case 4: buf.orientation.y = val; break;
			case 5: buf.orientation.z = val; break;
			case 6: buf.orientation.w = val; break;
		}
	}

	void put_flt(Ctx* ctx, double   val){
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: buf.position.x = val; break;
			case 1: buf.position.y = val; break;
			case 2: buf.position.z = val; break;
			case 3: buf.orientation.x = val; break;
			case 4: buf.orientation.y = val; break;
			case 5: buf.orientation.z = val; break;
			case 6: buf.orientation.w = val; break;
		}
	}

	void put_str(Ctx* ctx, String   str){
		double val = atof(str.c_str());
		int idx = ctx->idx.getNat(0);
		switch (idx){
			case 0: buf.position.x = val; break;
			case 1: buf.position.y = val; break;
			case 2: buf.position.z = val; break;
			case 3: buf.orientation.x = val; break;
			case 4: buf.orientation.y = val; break;
			case 5: buf.orientation.z = val; break;
			case 6: buf.orientation.w = val; break;
		}
	}

	void put_cmd(Ctx* stream, int type){
		if ( type == 0 ){
			pthread_mutex_lock(&this->mutex);
		} else if ( type == 1 ){
			stream->idx.atNat(0) += 1;
		} else if ( type == 2 ){
			stream->idx.atNat(0) = 0;
			pub.publish(buf);
		}
	}

	uint64_t size(){return 7;}
	String toStr(){}


	void callback(const geometry_msgs::Pose::ConstPtr& msg){
		event.put(topic).put("recv").put(msg->position.x).endl();
		this->buf.position    = msg->position;
		this->buf.orientation = msg->orientation;
		pthread_mutex_unlock(&this->mutex);
	}
};


struct RosCore : Unit {
	String name;
	ros::NodeHandle* node;
	pthread_t thread;

	RosCore(String name){
		this->name = name;
		int argc = 1;
		const char *argv[] = {"cmd",NULL};
		ros::init(argc, (char**)argv, "talker");
		this->node = new ros::NodeHandle();

		pthread_create(&thread, NULL, &thread_ros, this);
	}


	void openRandom (Random& pack){}
	void openStream (Stream& pack){}
	void get_raw(Ctx* ctx, void* src, size_t size){}
	void get_nat(Ctx* ctx, uint64_t& dst){}
	void get_int(Ctx* ctx, int64_t&  dst){}
	void get_flt(Ctx* ctx, double&   dst){}
	void get_str(Ctx* ctx, String&   dst){}
	void put_raw(Ctx* ctx, const void* src, size_t size){}
	void put_nat(Ctx* ctx, uint64_t val){}
	void put_int(Ctx* ctx, int64_t  val){}
	void put_flt(Ctx* ctx, double   val){}
	void put_str(Ctx* ctx, String   str){}
	void put_cmd(Ctx* ctx, int type){}
	uint64_t size(){}
	String toStr(){}

	static void* thread_ros(void* self){
		cout << "ros::spin\n";
		ros::spin();
	}
};*/



struct VetBufferRow : Unit {
	String format;
	std::vector<String> data;

	VetBufferRow(){
		data.resize(10);
	}

	void openRandom (Random& ctx){
		ctx.idx  = 0;
	}

	void openStream (Stream& ctx){
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



struct VetBufferUnit : Unit {
	volatile size_t writer_id, reader_id;
	pthread_mutex_t lock;
	std::vector<VetBufferRow> data;


	VetBufferUnit(){
		reader_id = writer_id = 0;
		pthread_mutex_init(&this->lock, NULL);
		this->data.resize(10);
	}

	~VetBufferUnit(){
		//pthread_mutex_destroy(&this->lock);
	}

	void openRandom (Random& pack){}
	void openStream (Stream& ctx){
		ctx.idx  = reader_id;
		ctx.idx.base = 1;
		ctx.data = new VetBufferRow();
	}

	void get_raw(Ctx* ctx, void* src, size_t size){}
	void get_nat(Ctx* ctx, uint64_t& dst){}

	void get_int(Ctx* ctx, int64_t&  dst){
		//row.get_int(ctx,dst);
	}

	void get_flt(Ctx* ctx, double&   dst){}
	void get_str(Ctx* ctx, String&   dst){}
	void put_raw(Ctx* ctx, const void* src, size_t size){}
	void put_nat(Ctx* ctx, uint64_t val){}
	void put_int(Ctx* ctx, int64_t  val){
		VetBufferRow* writer = (VetBufferRow*) ctx->data;
		writer->put_int(ctx,val);
	}
	void put_flt(Ctx* ctx, double   val){}
	void put_str(Ctx* ctx, String   str){}

	void put_cmd(Ctx* ctx, int type){
		if ( type == 0 ){
			VetBufferRow* reader = (VetBufferRow*) ctx->data;
			size_t idx = ctx->idx.getNat(-1);

			while ( idx >= writer_id ){
cout << "re " << idx << endl;
				usleep(200000);
			}

			pthread_mutex_lock(&this->lock);
			if ( idx >= reader_id ){
cout << "ra " << idx << endl;
				this->copyRow(reader,&this->data[bufId(idx)]);
				ctx->idx.atNat(-1) += 1;
			} else {
cout << "ro " << idx << endl;
				this->copyRow(reader,&this->data[bufId(reader_id)]);
				ctx->idx.atNat(-1) = reader_id;
			}
			pthread_mutex_unlock(&this->lock);
cout << reader->toStr() << endl;

			ctx->idx.atNat(0)   = 0;
		} else if ( type == 1 ){
			ctx->idx.atNat(0) += 1;
		} else if ( type == 2 ){
			pthread_mutex_lock(&this->lock);
			if ( writer_id >= this->data.size() ){
				reader_id += 1;
			}
cout << "w " << reader_id << " " << writer_id << endl;
			VetBufferRow* writer = (VetBufferRow*) ctx->data;
			this->copyRow(&this->data[bufId(writer_id)],writer);
			writer_id += 1;
			ctx->idx = writer_id;
			ctx->idx.putNat(0);
			ctx->idx.base = 1;
			pthread_mutex_unlock(&this->lock);
		}



	}

	uint64_t size(){return 0;}
	String toStr(){return "";}//row.toStr();}


	void copyRow(VetBufferRow* dst, VetBufferRow* src){
		size_t size = src->size();
		dst->data.resize( size );
		for ( size_t i=0; i<src->size(); i++ ){
			dst->data[i] = src->data[i];
		}
		dst->format = src->format;
	}

	size_t bufId(size_t pos){
		return pos%this->data.size();
	}
};








struct Teste {
	VetBufferUnit buf;
	pthread_t pub, sub[3];

	Teste(){
		pthread_create(&pub, NULL, publisher, this);
		pthread_create(&sub[0], NULL, subscriber, this);
		pthread_create(&sub[1], NULL, subscriber, this);
	}

	~Teste(){
		pthread_join(pub, NULL);
	}

	static void* publisher(void* arg){
		Teste* self = (Teste*) arg;
		Writer writer = self->buf;
		for ( int i=0; i<20; i++ ){
			writer.put(i).put(20).put(30).endl();
			sleep(1);
		}
		return NULL;
	}

	static void* subscriber(void* arg){
		Teste* self = (Teste*) arg;
		Reader reader = self->buf;
		for ( int i=0; i<20; i++){
			reader.readRow();
			usleep(500000);
		}
		return NULL;
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


	void openRandom (Random& pack){}
	void openStream (Stream& ctx){
		ctx.idx = 0;
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
	ItemString tmp("felipe bombardelli");

	ReaderRaw reader = tmp;

	cout << reader.get() << endl;
	cout << reader.get() << endl;
	cout << reader.get() << endl;

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
