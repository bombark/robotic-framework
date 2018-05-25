#include "core.hpp"

extern "C" void* ctx_new(){
	return new Ctx();
}

extern "C" int32_t ctx_stat(void* _ctx){
	return Stat().setMap().type16;
}


extern "C" char* ctx_idx_clear(void* _ctx){
	return "aaaa opa aaaa";
}

extern "C" void ctx_idx_put(void* ctx){
	Log("opa");
}

extern "C" void ctx_idx_get(void* ctx){
	Log("opa");
}


extern "C" void ctx_log(void* _ctx){
	CtxRandom* ctx = (CtxRandom*) _ctx;
	ctx->log();
}
