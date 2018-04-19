#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef std::string String;

using namespace std;



struct Unit;
struct Stream;
struct Random;


struct UnitApi {
};


struct Ctx {
	int    type;
	Unit*  unit;
	size_t idx;

	Ctx(){type = 0;}
	Ctx(Unit* unit){this->unit = unit;}
	Ctx(Unit& unit){this->unit = &unit;}

	bool isNull(){return unit==NULL;}
	bool isStream(){return type==1;}
	bool isRandom(){return type==2;}
};


struct Unit {
	virtual void openRandom (Random& pack)=0;
	virtual void openStream (Stream& pack)=0;

	virtual void read_bytes  (Ctx* ctx, void* dst, size_t size)=0;
	virtual void read_nat  (Ctx* ctx, void* dst, size_t sizebytes)=0;
	virtual void read_int  (Ctx* ctx, void* dst, size_t sizebytes)=0;
	virtual void read_flt  (Ctx* ctx, void* dst, size_t sizebytes)=0;

	virtual void write_bytes  (Ctx* ctx, const void* src, size_t size)=0;
	virtual void write_nat (Ctx* ctx, void* src, size_t sizebytes)=0;
	virtual void write_int (Ctx* ctx, void* src, size_t sizebytes)=0;
	virtual void write_flt (Ctx* ctx, void* src, size_t sizebytes)=0;
	virtual void write_str (Ctx* ctx, String str)=0;
	virtual void write_cmd (Ctx* ctx, String cmd)=0;

	virtual void   seek(Ctx* ctx, size_t pos)=0;
	virtual String toStr()=0;
};


struct UnitBasic : Unit {
	void read_nat  (Ctx* ctx, void* dst, size_t sizebytes){
		this->read_bytes(ctx,dst,sizebytes);
	}

	void read_int  (Ctx* ctx, void* dst, size_t sizebytes){
		this->read_bytes(ctx,dst,sizebytes);
	}

	void read_flt  (Ctx* ctx, void* dst, size_t sizebytes){
		this->read_bytes(ctx,dst,sizebytes);
	}

	void write_nat  (Ctx* ctx, void* src, size_t sizebytes){
		this->write_bytes(ctx,src,sizebytes);
	}

	void write_int  (Ctx* ctx, void* src, size_t sizebytes){
		this->write_bytes(ctx,src,sizebytes);
	}

	void write_flt  (Ctx* ctx, void* src, size_t sizebytes){
		this->write_bytes(ctx,src,sizebytes);
	}

	void write_str(Ctx* ctx, String str){
		this->write_bytes(ctx,str.c_str(),str.size());
	}
};





struct Stream : Ctx {

	Stream() : Ctx() {}
	Stream(Unit* unit) : Ctx(unit){this->type=1;idx=0;}
	Stream(Unit& unit) : Ctx(unit){this->type=1;idx=0;}


	uint8_t  getNat8(){
		uint8_t val;
		this->unit->read_nat(this, &val, sizeof(val));
		return val;
	}

	uint16_t getNat16(){
		uint16_t val;
		this->unit->read_nat(this, &val, sizeof(val));
		return val;
	}

	uint32_t getNat32(){
		uint32_t val;
		this->unit->read_nat(this, &val, sizeof(val));
		return val;
	}

	uint64_t getNat64(){
		uint64_t val;
		this->unit->read_nat(this, &val, sizeof(val));
		return val;
	}

	int8_t   getInt8(){
		int8_t val;
		this->unit->read_int(this, &val, sizeof(val));
		return val;
	}

	int16_t  getInt16(){
		int16_t val;
		this->unit->read_int(this, &val, sizeof(val));
		return val;
	}

	int32_t  getInt32(){
		int32_t val;
		this->unit->read_int(this, &val, sizeof(val));
		return val;
	}

	int64_t  getInt64(){
		int64_t val;
		this->unit->read_int(this, &val, sizeof(val));
		return val;
	}

	float    getFlt32(){
		float val;
		this->unit->read_flt(this, &val, sizeof(val));
		return val;
	}

	double   getFlt64(){
		double val;
		this->unit->read_flt(this, &val, sizeof(val));
		return val;
	}

	void getBytes(void* dst, size_t size){}


	void putNat8(uint8_t val){this->unit->write_nat(this, &val, sizeof(val));}
	void putNat16(uint16_t val){this->unit->write_nat(this, &val, sizeof(val));}
	void putNat32(uint32_t val){this->unit->write_nat(this, &val, sizeof(val));}
	void putNat64(uint64_t val){this->unit->write_nat(this, &val, sizeof(val));}

	void putInt8(int8_t val){this->unit->write_int(this, &val, sizeof(val));}
	void putInt16(int16_t val){this->unit->write_int(this, &val, sizeof(val));}
	void putInt32(int32_t val){this->unit->write_int(this, &val, sizeof(val));}
	void putInt64(int64_t val){this->unit->write_int(this, &val, sizeof(val));}

	void putFlt32(float val){this->unit->write_flt(this, &val, sizeof(val));}
	void putFlt64(double val){this->unit->write_flt(this, &val, sizeof(val));}

	void putStr(String val){this->unit->write_str(this,val);}
	void putBytes(const void* val, size_t size){this->unit->write_bytes(this,val, size);}

	void endl(){this->unit->write_cmd(this,"\n");}


	Stream& put(uint8_t val){this->putNat8(val);return *this;}
	Stream& put(uint16_t val){this->putNat16(val);return *this;}
	Stream& put(uint32_t val){this->putNat32(val);return *this;}
	Stream& put(uint64_t val){this->putNat64(val);return *this;}

	Stream& put(int8_t val){this->putInt8(val);return *this;}
	Stream& put(int16_t val){this->putInt16(val);return *this;}
	Stream& put(int32_t val){this->putInt32(val);return *this;}
	Stream& put(int64_t val){this->putInt64(val);return *this;}

	Stream& put(float val){this->putFlt32(val);return *this;}
	Stream& put(double val){this->putFlt64(val);return *this;}

	Stream& put(String val){this->putStr(val);return *this;}
	Stream& put(void* val, size_t size){this->putBytes(val,size);return *this;}

	size_t getIdx(){return idx;}
};




struct Random : Stream {
	Random() : Stream() {}
	Random(Unit* unit) : Stream(unit){this->type=2;}
	Random(Unit& unit) : Stream(unit){this->type=2;}

	inline Random& operator[](size_t idx){
		this->idx = idx;
		return *this;
	}
};








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
		out.idx = 0;
	}

	void openStream (Stream& out){
		out.idx = ftell(fd);
	}

	void read_bytes  (Ctx* ctx, void* dst, size_t sizebytes){
		if ( ctx->isRandom() ){
			Random* rnd = (Random*) ctx;
			this->seek(ctx, rnd->idx);
		}
		size_t size = fread(dst,1,sizebytes,this->fd);
		if ( ctx->isStream() ){
			Stream* stream = (Stream*) ctx;
			stream->idx += size;
		}
	}

	void write_bytes  (Ctx* ctx, const void* src, size_t sizebytes){
		if ( ctx->isRandom() ){
			Random* rnd = (Random*) ctx;
			this->seek(ctx, rnd->idx);
		}
		size_t size = fwrite(src,1,sizebytes,this->fd);
		if ( ctx->isStream() ){
			Stream* stream = (Stream*) ctx;
			stream->idx += size;
		}
	}

	void write_cmd (Ctx* ctx, String cmd){
	}

	void seek(Ctx* ctx, size_t pos){
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

	void read_bytes  (Ctx* ctx, void* dst, size_t size){
		base.getBytes(dst,size);
	}



	void write_bytes  (Ctx* ctx, const void* src, size_t size){
		base.putBytes(src,size);
	}

	void write_nat(Ctx* ctx, void* src, size_t size){
		this->putEndVar();
		char buf[64];
		if ( size == 1 ){
			uint8_t* val = (uint8_t*) src;
			sprintf(buf,"%u",*val);
		} else if ( size == 2 ){
			uint16_t* val = (uint16_t*) src;
			sprintf(buf,"%u",*val);
		} else if ( size == 4 ){
			uint32_t* val = (uint32_t*) src;
			sprintf(buf,"%u",*val);
		} else if ( size == 8 ){
			uint64_t* val = (uint64_t*) src;
			sprintf(buf,"%lu",*val);
		} else {
			return ;
		}
		base.putBytes(buf,strlen(buf));
	}

	void write_int(Ctx* ctx, void* src, size_t size){
		this->putEndVar();
		char buf[64];
		if ( size == 1 ){
			int8_t* val = (int8_t*) src;
			sprintf(buf,"%c",*val);
		} else if ( size == 2 ){
			int16_t* val = (int16_t*) src;
			sprintf(buf,"%u",*val);
		} else if ( size == 4 ){
			int32_t* val = (int32_t*) src;
			sprintf(buf,"%d",*val);
		} else if ( size == 8 ){
			int64_t* val = (int64_t*) src;
			sprintf(buf,"%ld",*val);
		} else {
			return ;
		}
		base.putBytes(buf,strlen(buf));
	}

	void write_flt(Ctx* ctx, void* src, size_t size){
		this->putEndVar();
		char buf[64];
		if ( size == 4 ){
			float* val = (float*) src;
			sprintf(buf,"%f",*val);
		} else if ( size == 8 ){
			double* val = (double*) src;
			sprintf(buf,"%lf",*val);
		} else {
			return ;
		}
		base.putBytes(buf,strlen(buf));
	}

	void write_str(Ctx* ctx, String str){
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


	void seek(Ctx* ctx, size_t pos){}
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

};



struct Pose {
	float x,y,z;
};


struct RosPose : Unit {
	Pose pose;

	void openRandom (Random& pack){}
	void openStream (Stream& pack){}
	void read_bytes  (Ctx* ctx, void* dst, size_t size){}
	void read_nat  (Ctx* ctx, void* dst, size_t sizebytes){}
	void read_int  (Ctx* ctx, void* dst, size_t sizebytes){}
	void read_flt  (Ctx* ctx, void* dst, size_t sizebytes){}
	void write_bytes  (Ctx* ctx, const void* src, size_t size){}

	void write_nat (Ctx* ctx, void* src, size_t sizebytes){
		if ( ctx->idx == 0 ){
			if ( sizebytes == 4 ){
				uint32_t* val = (uint32_t*) src;
				pose.x = *val;
			} else if ( sizebytes == 8 ){
				uint64_t* val = (uint64_t*) src;
				pose.x = *val;
			}
		}
	}

	void write_int (Ctx* ctx, void* src, size_t sizebytes){
		if ( ctx->idx == 0 ){
			if ( sizebytes == 4 ){
				uint32_t* val = (uint32_t*) src;
				pose.x = *val;
			} else if ( sizebytes == 8 ){
				uint64_t* val = (uint64_t*) src;
				pose.x = *val;
			}
		}
	}



	void write_flt (Ctx* ctx, void* src, size_t sizebytes){}
	void write_str (Ctx* ctx, String str){}
	void write_cmd (Ctx* ctx, String cmd){}
	void   seek(Ctx* ctx, size_t pos){}
	String toStr(){}

};





int main(){
	//FsFile  file;//("teste.txt","w");
	//TextDrv text(file);

	RosPose file;

	Random rnd(file);
	rnd[0].put(130);
	rnd[1].put(20);
	rnd[2].put(30);

	cout << file.pose.x << endl;

	/*int a = rnd[0].getInt8();
	int b = rnd[4].getInt8();
	int c = rnd[8].getInt32();
	cout << a << " " << b << endl;*/

	/*Stream out(text);
	out.put(10).put(20).put("fel\nipe\n bombar\ndelli").endl();
	out.put(10).put(22).put(32).endl();
	out.put(10).put(42).put(12).endl();*/



	//cout << out.getIdx() << endl;*/
}
