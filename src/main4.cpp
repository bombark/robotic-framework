#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>

typedef std::string String;

using namespace std;



struct Unit;
struct Stream;
struct Random;



struct Args {
	size_t size;
	char   format[4];
	union {
		uint64_t nat64[4];
		int64_t  int64[4];
		double   flt64[4];
	};
	String str[4];

	Args(){this->clear();}

	void clear(){this->size = 0;}


	void putNat(uint64_t val){
		nat64[size]  = val;
		format[size++] = 'u';
	}

	void putStr(String val){
		str[size] = val;
		format[size++] = 's';
	}

	uint64_t& atNat(int id){return nat64[id];}

	uint64_t getNat(int id){
		switch ( format[id] ){
			case 'u': return nat64[id];
			//case 's': return str[id];
			case 'd': return int64[id];
			case 'f': return flt64[id];
		}
	}

	String   getStr(int id){
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
	}
};



struct Ctx {
	int    type;
	Unit*  unit;
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
	virtual void openRandom (Random& pack)=0;
	virtual void openStream (Stream& pack)=0;

	virtual void get_raw(Ctx* ctx, void* src, size_t size)=0;
	virtual void get_nat(Ctx* ctx, uint64_t& dst)=0;
	virtual void get_int(Ctx* ctx, int64_t&  dst)=0;
	virtual void get_flt(Ctx* ctx, double&   dst)=0;
	virtual void get_str(Ctx* ctx, String&   dst)=0;

	virtual void put_raw(Ctx* ctx, const void* src, size_t size)=0;
	virtual void put_nat(Ctx* ctx, uint64_t val)=0;
	virtual void put_int(Ctx* ctx, int64_t  val)=0;
	virtual void put_flt(Ctx* ctx, double   val)=0;
	virtual void put_str(Ctx* ctx, String   str)=0;
	virtual void put_cmd(Stream* stream, int type)=0;

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
		this->unit->put_cmd(this,0);
		this->unit->put_nat(this,val);
		this->unit->put_cmd(this,1);
	}

	void putInt(int64_t  val){
		this->unit->put_cmd(this,0);
		this->unit->put_int(this,val);
		this->unit->put_cmd(this,1);
	}

	void putFlt(double   val){
		this->unit->put_cmd(this,0);
		this->unit->put_flt(this,val);
		this->unit->put_cmd(this,1);
	}

	void putStr(String val){
		this->unit->put_cmd(this,0);
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

	void put_cmd(Stream* ctx, int type){
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

	void put_cmd(Ctx* ctx){}

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


	void write_cmd (Ctx* ctx, String cmd){
		base.put(endl);
		this->first = true;
	}


	void putEndVar(){
		if ( this->first ){
			this->first = false;
		} else {
			base.put(endv);
		}
	}

	void get_str(Ctx* ctx, String&   dst){}

	void put_cmd(Ctx* ctx){
		base.put(endl);
		this->first = true;
	}

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

	void put_cmd(Ctx* ctx){
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





#include "ros/ros.h"
#include <geometry_msgs/Pose.h>



struct RosPose : Unit {
	String topic;
	ros::Publisher  pub;
	ros::Subscriber sub;
	geometry_msgs::Pose buf;

	RosPose(ros::NodeHandle& n, String topic, char mode){
		this->topic = topic;
		if ( mode == 'w' )
			this->pub = n.advertise<geometry_msgs::Pose>(topic, 1000);
		else if ( mode == 'r' )
			this->sub = n.subscribe(topic, 1000, &RosPose::callback, this);
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

	void put_cmd(Stream* stream, int type){
		if ( type == 1 ){
			stream->idx.atNat(0) += 1;
		} else if ( type == 2 ){
			stream->idx.atNat(0) = 0;
			pub.publish(buf);
		}
	}

	uint64_t size(){return 7;}
	String toStr(){}


	void callback(const geometry_msgs::Pose::ConstPtr& msg){
cout << "aaaaa\n";
		this->buf.position    = msg->position;
		this->buf.orientation = msg->orientation;
	}
};



/*struct Translate : Unit {
	Random data;
	vector<size_t> src;
	vector<size_t> dst;
	Translate(){
	}
};*/



int main(int argc, char** argv){
	if ( argc == 1 ){
		ros::init(argc, argv, "talker");
		ros::NodeHandle node;

		RosPose ppose(node,"chatter",'w');
		Stream pose = ppose;
		for(int i=0; i<10; i++){
			pose.put(30).put("66").put(20).put(33.333).put(40).put(50).put(60).endl();
			sleep(1);
		}

	} else {
		ros::init(argc, argv, "receiver");
		ros::NodeHandle node;

		RosPose ppose(node,"chatter",'r');
		MsgStream pose = ppose;
		for(int i=0; i<4; i++){
			pose.read();
			cout << pose[0].getNat() << endl;
			ros::spinOnce();
			sleep(1);

		}
	}


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
