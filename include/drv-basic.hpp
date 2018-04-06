#include <vector>
#include <map>




struct VetStr : Vet {
	std::vector<Stat>   stats;
	std::vector<string> data;

	VetStr(){
		api.to_str  = ctx_to_str;
		api.stat    = ctx_stat;
	}

	void putNat(uint64_t val){
		char buf[64]; sprintf(buf,"%lu",val);
		this->data.push_back(buf);
		this->stats.push_back( Stat().setNat() );
	}

	void putInt(int64_t  val){
		char buf[64]; sprintf(buf,"%ld",val);
		this->data.push_back(buf);
		this->stats.push_back( Stat().setInt() );
	}

	void putFlt(double   val){
		char buf[64]; sprintf(buf,"%lf",val);
		this->data.push_back(buf);
		this->stats.push_back( Stat().setFlt() );
	}

	void putStr(String val){
		this->data.push_back(val);
		this->stats.push_back( Stat().setStr() );
	}

	virtual void putColl(Coll* coll){
		/*if ( coll->stat.isVet() ){
			String val = this->ctx.api.serialize(coll);
			this->data.push_back(val);
			this->stats.push_back( Stat().setVet() );
		} else if ( coll->stat.isMap() ){
			String val = this->ctx.api.serialize(coll);
			this->data.push_back(val);
			this->stats.push_back( Stat().setMap() );
		}*/
	}

	// virtuals functions
	size_t size(){return data.size();}


	// ctx functions
	/*void buildCtx(Ctx& ctx){
		Coll::buildCtx(ctx);
	}*/

	static Stat ctx_stat(Ctx* ctx){
		VetStr* self = (VetStr*) ctx->coll;
		if ( ctx->idx_i >= self->size() )
			return Stat(0);
		return self->stats[ctx->idx_i];
	}

	static String ctx_to_str(Ctx* ctx){
		VetStr* self = (VetStr*) ctx->coll;
		return self->data[ctx->idx_i];
	}
};



struct MapStrIt : VetFixed {
	std::map<String,String>::iterator it;
	std::map<String,String>::iterator end;

	void buildCtx(Ctx& ctx){
		Coll::buildCtx(ctx);
		ctx.coll = this;
		//ctx.api->to_str = ctx_to_str;
	}

	Cursor buildCursor(){
		Cursor cur;
		/*cur.ctx.coll = this;
		cur.ctx.api.seek_int = cursor_seek_int;
		cur.ctx.api.to_coll  = cursor_to_coll;
		cur.ctx.api.stat     = cursor_stat;*/
		return cur;
	}

	size_t size(){return 2;}

	static String ctx_to_str(Ctx* ctx){
//cout << "ctx_to_str\n";
		MapStrIt* self = (MapStrIt*) ctx->coll;
		if ( ctx->idx_i == 0 )
			return self->it->first;
		if ( ctx->idx_i == 1 )
			return self->it->second;
		return "";
	}

	static Stat ctx_stat(Ctx* ctx){
		MapStrIt* self = (MapStrIt*) ctx->coll;
		if ( ctx->idx_i == 0 )
			return Stat().setStr();
		if ( ctx->idx_i == 1 )
			return Stat().setStr();
		return Stat(0);
	}


	static Stat cursor_stat(Ctx* ctx){
		MapStrIt* self = (MapStrIt*) ctx->coll;
		if ( self->it == self->end )
			return Stat(0);
		else {
			return Stat().setVet();
		}
	}

	static void cursor_seek_int(Ctx* ctx, int64_t idx){
		MapStrIt* self = (MapStrIt*) ctx->coll;
		++self->it;
	}

	static Ctx cursor_to_coll(Ctx* ctx){
		MapStrIt* self = (MapStrIt*) ctx->coll;
		Ctx res;
		self->buildCtx(res);
		return res;
	}
};


struct MapStr : Map {
	std::map<String,String> data;

	MapStr(){
		api.stat    = ctx_stat;
		api.to_str  = ctx_to_str;
		api.set_str = ctx_set_str;
	}

	Cursor begin(){
		MapStrIt* it = new MapStrIt();
		it->it = this->data.begin();
		it->end = this->data.end();
		return it->buildCursor();
	}


	// virtuals functions
	size_t size(){return data.size();}

	static Stat ctx_stat(Ctx* ctx){
		MapStr* self = (MapStr*) ctx->coll;
		std::map<String,String>::iterator it = self->data.find(ctx->idx_s);
		if ( it != self->data.end() ){
			Stat stat;
			stat.setStr();
			return stat;
		} else {
			return Stat(0);
		}
	}

	static void ctx_set_str(Ctx* ctx, String val){
		MapStr* self = (MapStr*) ctx->coll;
		std::map<String,String>::iterator it = self->data.find(ctx->idx_s);
		if ( it != self->data.end() ){
			it->second = val;
		} else {
			self->data.insert( std::pair<String,String>(ctx->idx_s,val) );
		}
	}

	static String ctx_to_str(Ctx* ctx){
		MapStr* self = (MapStr*) ctx->coll;
		std::map<String,String>::iterator it = self->data.find(ctx->idx_s);
		if ( it != self->data.end() ){
			return it->second;
		}
		return "";
	}
};
