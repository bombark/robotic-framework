#include "core.hpp"
#include <yaml-cpp/yaml.h>


struct YamlIterator : StreamData {
	YAML::const_iterator it;
	YAML::const_iterator end;

	YamlIterator(YAML::Node node){
		this->it  = node.begin();
		this->end = node.end();
	}

	String   val_get_str (CtxStream& ctx){
		//if ( this->it->second.IsScalar() )
		return this->it->second.as<String>();
	}

	CtxRandom val_get_vet (CtxStream& ctx);

	Stat itr_stat(CtxStream& ctx){
		YAML::Node n = this->it->second;
		if ( n.IsScalar() )
			return Stat().setItem();
		if ( n.IsSequence() )
			return Stat().setVet();
		if ( n.IsMap() )
			return Stat().setMap();
	}

	void itr_begin(CtxStream& ctx){
		ctx.idx = this->itr_is_ok(ctx) ? it->first.as<String>() : "";
	}

	void itr_prev(CtxStream& ctx){
		//--it;
		ctx.idx = this->itr_is_ok(ctx) ? it->first.as<String>() : "";
	}

	void itr_next(CtxStream& ctx){
		++it;
		ctx.idx = this->itr_is_ok(ctx) ? it->first.as<String>() : "";
	}

	bool itr_is_ok(CtxStream& ctx){
		return this->it != this->end;
	}
};



struct YamlNode : UnitTree {
	YAML::Node node;

	YamlNode(String text){
		this->node = YAML::Load(text);
	}

	YamlNode(const YAML::Node node){
		this->node = node;
	}

	String classe(){return "Yaml/Node";}

	Stat     stat(){
		if ( this->node.IsNull() )
			return Stat(0);
		if ( this->node.IsScalar() )
			return Stat().setItem();
		if ( this->node.IsMap() )
			return Stat().setMap();
		if ( this->node.IsSequence() )
			return Stat().setVet();
		return Stat(0);
	}

	uint64_t size(){return node.size();}




	Stat     random_stat   (CtxRandom* ctx){
		YAML::Node coll; String field;
		return this->findNode(*ctx, coll, field);
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

	void root_open_stream (CtxStream& dst, char mode){
		dst.stream_data = new YamlIterator(this->node);
	}

	void node_open_stream (const CtxRandom* ctx, CtxStream& dst, char mode){
		if ( ctx->idx.size == 0 ){
			dst.stream_data = new YamlIterator(this->node);
		} else {
			String field = ctx->idx.getStr(0);
			dst.stream_data = new YamlIterator(this->node[field]);
		}
	}

	void root_open_random(CtxRandom& dst, char mode){
		dst.unit = new YamlNode( this->node );
	}

	void random_open_random (const CtxRandom* ctx, CtxRandom& dst, char mode){
		String field = ctx->idx.getStr(0);
		dst.unit = new YamlNode( this->node[field] );
	}

	uint64_t random_get_raw (CtxRandom* ctx, void* dst, size_t size){return 0;}
	uint64_t random_put_raw (CtxRandom* ctx, const void* src, size_t size){return 0;}

	uint64_t random_get_nat (CtxRandom* ctx){
		YAML::Node coll; String field;
		Stat type = this->findNode(*ctx, coll, field);
		if ( type.isItem() ){
			return coll[field].as<uint64_t>();
		}
		return 0;
	}

	int64_t  random_get_int (CtxRandom* ctx){
		YAML::Node coll; String field;
		Stat type = this->findNode(*ctx, coll, field);
		if ( type.isItem() ){
			return coll[field].as<int64_t>();
		}
		return 0;
	}

	double   random_get_flt (CtxRandom* ctx){
		YAML::Node coll; String field;
		Stat type = this->findNode(*ctx, coll, field);
		if ( type.isItem() ){
			return coll[field].as<double>();
		}
		return 0;
	}

	void     random_get_str (CtxRandom* ctx, String& dst){
		YAML::Node coll; String field;
		Stat type = this->findNode(*ctx, coll, field);
		if ( type.isItem() ){
			dst += coll[field].as<String>();
		} else if ( type.isColl() ){
			cout << coll << endl;
		}
	}

	void     random_put_nat (CtxRandom* ctx, uint64_t val){

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


protected:
	Stat findNode( const CtxRandom& ctx, YAML::Node& out, String& out_field){
		return this->findNodeRec(ctx,this->node,0,out,out_field);
	}

	Stat findNodeRec( const CtxRandom& ctx, YAML::Node last, size_t i, YAML::Node& out, String& out_field){
		if ( ctx.idx.size == 0 ){
			out = this->node;
			out_field.clear();
			return Stat().setColl();
		}

		if ( i >= ctx.idx.size ){
			return Stat(0);
		}

		YAML::Node cur;
		if ( last.IsSequence() ){
			uint64_t field    = ctx.idx.getNat(i);
			cur = last[field];
		} else if ( last.IsMap() ){
			String field    = ctx.idx.getStr(i);
			cur = last[field];
		}

		if ( cur ){
			if ( cur.IsScalar() ){
				if ( ctx.idx.size < i )
					return Stat(0);
				out = last;
				out_field = ctx.idx.getStr(i);
				return Stat().setItem();
			} else if ( cur.IsSequence() ){
				if ( i+1 == ctx.idx.size ){
					out = cur; out_field.clear();
					return Stat().setVet();
				} else {
					return this->findNodeRec(ctx,cur,i+1,out,out_field);
				}
			}  else if ( cur.IsMap() ){
				if ( i+1 == ctx.idx.size ){
					out = cur; out_field.clear();
					return Stat().setMap();
				} else {
					return this->findNodeRec(ctx,cur,i+1,out,out_field);
				}
			}
			return Stat(0);
		}
		out = last;
		out_field = ctx.idx.getStr(i);
		return Stat(0);
	}
};




CtxRandom YamlIterator::val_get_vet (CtxStream& ctx){
	CtxRandom res = new YamlNode(this->it->second);
	res.log();
	return res;
}
