#!/usr/bin/python

import ctypes

URFC = ctypes.CDLL("./liburfc.so")
pChar = ctypes.c_char_p
pVoid = ctypes.c_void_p
URFC.ctx_idx_clear.restypes = pChar


class Ctx:
	def __init__(self):
		self.ptr = URFC.ctx_new()

	def stat(self):
		a = URFC.ctx_stat(self.ptr)
		print(self.ptr)

	def clearIdx(self):
		a = URFC.ctx_idx_clear(self.ptr)
		print(self.ptr)

	def log(self):
		URFC.ctx_log(self.ptr)

	#def __getitem__(self, idx):
	#	self.idx = idx
	#	return self;

	#def __setattr__(self, attr, value):
	#	print attr,value

	#def __setitem__(self, idx, val):
	#	self.idx = idx
	#	self.unit.ctx_set(self,val)


class Unit(object):
	def __init__(self):
		self.classe = "Unit"
		#self.data    = {}
		#self.proc    = []
		#self.ctrl    = []
		#self.classes = {}

	def start(self):
		raise("not implemented")

	def stop(self):
		raise("not implemented")

	def call(self):
		raise("not implemented")


class DataUnit(Unit):
	def __init__(self):
		Unit.__init__(self)
		self.data = "felipe bombardelli"

	def stat(self):
		ctx = Ctx(self)
		return ctx.stat()

	def ctx(self):
		return Ctx(self)

	def ctx_stat(self,ctx):
		raise("not implemented")

	def ctx_to_str(self,ctx):
		raise("not implemented")

	def ctx_set(self,ctx,val):
		raise("not implemented")


class ItemStd(DataUnit):
	def __init__(self):
		DataUnit.__init__(self)
		self.data = ""

	def ctx_stat(self,ctx):
		return Stat().setItem()

	def ctx_to_str(self,ctx):
		return self.data

	def ctx_set(self,ctx,val):
		self.data = val




class MapStd(DataUnit):
	def __init__(self):
		DataUnit.__init__(self)
		self.data = {}

	def ctx_stat(self,ctx):
		if len(ctx.idx) == 0:
			return Stat().setMap()
		else:
			item = self.data[ idx[0] ]
			return Stat().setItem()
			#self.data[ idx[0] ]

	def ctx_to_str(self,ctx):
		if len(ctx.idx) == 0:
			return ""
		else:
			return self.data[ ctx.idx[0] ]

	def ctx_set(self,ctx,val):
		self.data[ ctx.idx ] = val





class UnitFromLib(Unit):
	def __init__(self,classe,library_path):
		Unit.__init__(self)
		self.classe  = classe
		#self.handler = ctypes.CDLL(library_path)
		self.object  = "ddd"

	def load(self,library_path):
		print("loading")

	def stat(self):
		self.handler.urf_stat(self.object)

	def start(self):
		self.handler.urf_start(self.object)

	def stop(self):
		self.handler.urf_stop(self.object)






class Robot(Unit):
	def __init__(self):
		Unit.__init__(self)
		print("aaa")

	def start(self):
		print("opa")



print("opa")
ctx = Ctx()
#ctx.clearIdx()
ctx.log()
#value.stat()


#ctx = Ctx();
#print ctx["aaaa"].toStr()


#robot = Robot()
#robot.data["system"] = UnitFromLib("filesystem","libudm-filesystem")
#robot.data["user"]   = UnitFromLib("filesystem","libudm-filesystem")
#robot.data["ros"]    = UnitFromLib("ros-topic","libudm-ros")
#robot.ctrl["ros"]    = UnitFromLib("teste","libteste.so")
#it = robot.data["system"].list();
#robot.start()
