#!/usr/bin/python

import ctypes

class Stat:
	def __init__(self):
		self.type = 0

	def isNull(self): return self.type == 0x0000;
	def isItem(self): return (self.type&0x0010) == 0x0010;
	def isColl(self): return (self.type&0x0020) == 0x0020;

	def isVet(self): return (self.type&0x003F) == 0x0021;
	def isMap(self): return (self.type&0x003F) == 0x0022;

	def setNull(self): self.type = 0; return self;
	def setItem(self): self.type = 0x0010; return self;
	def setNat (self): self.type = 0x0011; return self;
	def setStr (self): self.type = 0x0012; return self;
	def setColl(self): self.type = 0x0020; return self;
	def setVet (self): self.type = 0x0021; return self;
	def setMap (self): self.type = 0x0022; return self;


	def __str__(self):
		if self.isNull():
			return "[Null]"
		if self.isItem():
			return "[Item]"
		if self.isVet():
			return "[Coll:Vet]"
		if self.isMap():
			return "[Coll:Map]"
		if self.isColl():
			return "[Coll]"
		return "[Err]"


class Ctx:
	def __init__(self, unit):
		self.idx = []
		self.unit = unit

	def stat(self):
		return self.unit.ctx_stat(self)

	def clear(self):
		raise("aaa")

	def toStr(self):
		return self.unit.ctx_to_str(self)

	def toInt(self):
		# unit(idx,)
		return 0;

	def size(self):
		return self.unit.ctx_size(self)

	def rawsize(self):
		return 0;

	def set(self, val):
		self.unit.ctx_set(self,val)

	def __str__(self):
		return self.unit.ctx_to_str(self)

	def __getitem__(self, idx):
		self.idx = idx
		return self;

	#def __setattr__(self, attr, value):
	#	print attr,value

	def __setitem__(self, idx, val):
		self.idx = idx
		self.unit.ctx_set(self,val)


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



mapa_unit = MapStd()
mapa = mapa_unit.ctx()
print mapa.stat()



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
