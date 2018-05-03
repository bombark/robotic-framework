#include "core.hpp"
#include <yaml-cpp/yaml.h>


struct NodeIteratorData {
	YAML::const_iterator it;
	YAML::const_iterator end;

	NodeIteratorData(YAML::Node node){
		this->it  = node.begin();
		this->end = node.end();
	}

	Stat stat(){
		YAML::Node n = this->it->second;
		if ( n.IsScalar() )
			return Stat().setItem();
		if ( n.IsSequence() )
			return Stat().setVet();
		if ( n.IsMap() )
			return Stat().setMap();
	}

	void next(CtxStream* ctx){
		++it;
		ctx->idx = this->isOk() ? it->first.as<String>() : "";
	}

	bool isOk(){
		return this->it != this->end;
	}
};



struct YamlNode : UnitTree {
	YAML::Node node;

	YamlNode(String text){
		this->node = YAML::Load(text);
	}

	YamlNode(YAML::Node node){
		this->node = node;
	}

	Stat     stat(){return Stat().setMap();}

	uint64_t size(){return node.size();}

	Stat      stream_stat_line (CtxStream* ctx){
		NodeIteratorData* data = (NodeIteratorData*) ctx->data;
		return data->stat();
	}

	void      stream_save_line (CtxStream* ctx, uint8_t level){}

	void      stream_load_line (CtxStream* ctx){
		NodeIteratorData* data = (NodeIteratorData*) ctx->data;
		data->next(ctx);
	}

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


	Stat     random_stat   (CtxRandom* ctx){
		String field = ctx->idx.getStr(0);
		YAML::Node node = this->node[field];
		if ( node ){
			if ( node.IsScalar() )
				return Stat().setItem();
			if ( node.IsSequence() )
				return Stat().setVet();
			if ( node.IsMap() )
				return Stat().setMap();
			//if ( node.isStr() ){
			//	return Stat().setStr();
			//}
		} else {
			return Stat(0);
		}
	}


	void     random_erase  (CtxRandom* ctx, bool   index){
		String field = ctx->idx.getStr(0);
		if ( index == true ){
			//this->node.reset(field);
			this->node[field] = "";
		} else {
			this->node[field] = "";
		}
	}

	void     random_resize (CtxRandom* ctx, size_t size){}

	uint64_t random_size   (CtxRandom* ctx){
		String field = ctx->idx.getStr(0);
		return this->node[field].size();
	}

	void random_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode){
		if ( ctx == NULL ){
			dst.data = new NodeIteratorData(node);
		} else {
			if ( ctx->idx.size == 0 ){
				dst.data = new NodeIteratorData(this->node);
			} else {
				String field = ctx->idx.getStr(0);
				dst.data = new NodeIteratorData(this->node[field]);
			}
		}
	}

	void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){
		String field = ctx->idx.getStr(0);
		dst.unit = new YamlNode( this->node[field] );
	}

	uint64_t random_get_raw (CtxRandom* ctx, void* dst, size_t size){return 0;}
	uint64_t random_put_raw (CtxRandom* ctx, const void* src, size_t size){return 0;}

	uint64_t random_get_nat (CtxRandom* ctx){
		String field = ctx->idx.getStr(0);
		return this->node[field].as<uint64_t>();
	}

	int64_t  random_get_int (CtxRandom* ctx){
		String field = ctx->idx.getStr(0);
		return this->node[field].as<int64_t>();
	}

	double   random_get_flt (CtxRandom* ctx){
		String field = ctx->idx.getStr(0);
		return this->node[field].as<double>();
	}

	void     random_get_str (CtxRandom* ctx, String& dst){
		String field = ctx->idx.getStr(0);
		dst += this->node[field].as<String>();
	}

	void     random_put_nat (CtxRandom* ctx, uint64_t val){
		String field = ctx->idx.getStr(0);
		this->node[field] = val;
	}
	void     random_put_int (CtxRandom* ctx, int64_t  val){
		String field = ctx->idx.getStr(0);
		this->node[field] = val;
	}
	void     random_put_flt (CtxRandom* ctx, double   val){
		String field = ctx->idx.getStr(0);
		this->node[field] = val;
	}
	void     random_put_str (CtxRandom* ctx, String   val){
		String field = ctx->idx.getStr(0);
		this->node[field] = val;
	}

	void     random_make (CtxRandom* ctx, Stat type){
	}

	void     random_make_item (CtxRandom* ctx, size_t size){
	}

};
