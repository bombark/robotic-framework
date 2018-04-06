#include "core.hpp"
#include "drv-basic.hpp"
//#include "drv-filesystem.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <yaml-cpp/yaml.h>

using namespace std;






struct FsCursor : Cursor {
	DIR*   dir;
	VetStr row;

	FsCursor(){
		this->dir = opendir("./");
	}

	MapPtr rowMap(){
	}

	VetPtr rowVet(){
		return row.toVet();
	}

};



/*
struct CursorDrv {
	DIR* dir;
	CollPtr ptr;
	vector<string> header;
	string first;

	CursorDrv(){
		this->dir = opendir("./");
	}

	static String ctx_to_str(Ctx* ctx){
		CursorDrv* self = ctx->coll;
		if ( ctx->idx_i == 0 ){
			return self->first;
		} else if ( ctx->idx_i == 1 ) {
			return self->ptr[ self->first ].toStr();
		}
		return ""
	}

	static void ctx_set_str(Ctx* ctx, String val){
	}
};


struct CursorPtr : Ctx {

};


struct Dir {
	DIR* dir;

	add_seek(Cursor2* cur){
		readdir(dir);
		cur->row[0] = first;
		cur->row[1] = second;
	}

	void select(VetPtr fields)
};


struct SubItem {
	Api api;
};

struct MapYamlRow : Vet {
	YAML::const_iterator it;
	YAML::const_iterator end;

	MapYamlIt(YAML::Node node){
		this->it  = node.begin();
		this->end = node.end();
		this->api.seek_int = ctx_seek_int;
		this->api.to_str   = ctx_to_str;
	}

	static String ctx_to_str(Ctx* ctx){
		MapYamlIt* self = (MapYamlIt*) ctx->data;
		return self->it->second.as<String>();
	}

	static void ctx_seek_int(Ctx* ctx, int64_t idx){
		MapYamlIt* self = (MapYamlIt*) ctx->data;
		++self->it;
		ctx->path_s = self->it->first.as<String>();
	}
};


struct MapYaml : Map {
	YAML::Node node;

	MapYaml() : Map() {
		this->node = YAML::LoadFile("data.yml");
		this->buildCtx(this->ctx);
	}

	MapYaml(YAML::Node node) : Map() {
		this->node = node;
		this->buildCtx(this->ctx);
	}

	Ctx begin2(){
		Ctx res;
		res.coll = this;
		MapYamlIt* it = new MapYamlIt(this->node);
		res.data = it;
		res.api = it->api;
		return res;
	}

	size_t size(){return 0;}



	void buildCtx(Ctx& ctx){
		this->ctx.api.to_str   = ctx_to_str;
		this->ctx.api.seek_str = ctx_seek_str;
		this->ctx.api.to_coll  = ctx_to_coll;
	}

	static String   ctx_to_str(Ctx* ctx){
		MapYaml* self = (MapYaml*) ctx->coll;
		return self->node[ ctx->idx_s ].as<String>();
	}

	static Ctx    ctx_to_coll(Ctx* ctx){
		MapYaml* self = (MapYaml*) ctx->coll;
		MapYaml* res = new MapYaml( self->node["posts"] );
		return res->ctx;
	}

	static void   ctx_seek_nat(Ctx* ctx, uint64_t idx){
	}

	static void   ctx_seek_int(Ctx* ctx, int64_t idx){
	}

	static void   ctx_seek_str(Ctx* ctx, String   idx){
		ctx->idx_s = idx;
	}


	YAML::Node find( String url ){

	}
};*/





int main(){
	VetStr vet;
	vet << "opa1" << "opa2" << "opa3";
	cout << vet.size() << endl;

	VetPtr vetptr = &vet;
	if ( vetptr.isVet() ){
		cout << "vet\n";
	} else {
		cout << "map\n";
	}

	cout << vetptr[2].toStr() << endl;

	//cout << vet[1].size() << endl;



	//cout << vet[1].toStr() << endl;

	//vet[0];// = "ddd";
	//cout << vetptr << endl;


	//MapPtr posts = yml["posts"].toColl();
	//cout << posts["name"].toStr() << endl;
	//MapIt it = yml.begin2();

	/*it.seekInt(1);
	cout << it.toStr() << endl;

	it.seekInt(1);
	cout << it.toStr() << endl;

	/ *FileSystem fs("./person");

	for (MapIt it = fs.begin(); it.isOk(); ++it ){
		cout << it.id() << endl;
		//cout << it.item().toStr() << endl;
	}*/



	/*for ( int i=0; i<vet.size(); i++){
		cout << vet[i].toStr() << endl;
	}*/

	/*MapStr map;
	map["name"]  = "felipe bombardelli";
	map["idade"] = "dddd";
	cout << map << endl;*/

	/*Ctx ctx = map.begin();
	cout << ctx.toStr() << endl;

	ctx.next();
	cout << ctx.toStr() << endl;*/

	/*for ( Cursor it=map.begin(); it.isOk(); ++it ){
		Ctx row = it.row();
		cout << row[0].toStr() << ": " << row[1].toStr() << endl;
	}*/





	return 0;
}






/**/
