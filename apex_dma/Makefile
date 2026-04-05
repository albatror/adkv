CXX=g++
CXXFLAGS=-I./memflow_lib/memflow-ffi/ -Wno-multichar
LIBS=./memflow_lib/target/release/libmemflow_ffi.a -lm -ldl -lpthread

OUTDIR=./build
OBJDIR=$(OUTDIR)/obj

$(shell mkdir -p $(OBJDIR))

%.o: %.cpp
	$(CXX) -c -o $(OBJDIR)/$@ $< $(CXXFLAGS)

apex_dma: apex_dma.o Game.o Math.o memory.o offsets_dynamic.o StuffBot.o ItemManager.o
	$(CXX) -o $(OUTDIR)/$@ $(OBJDIR)/apex_dma.o $(OBJDIR)/Game.o $(OBJDIR)/Math.o $(OBJDIR)/memory.o $(OBJDIR)/offsets_dynamic.o $(OBJDIR)/StuffBot.o $(OBJDIR)/ItemManager.o $(CXXFLAGS) $(LIBS)
	cp items.ini $(OUTDIR)/
	cp weapons.ini $(OUTDIR)/

.PHONY: all
all: apex_dma

.DEFAULT_GOAL := all

clean:
	rm -rf $(OUTDIR)
