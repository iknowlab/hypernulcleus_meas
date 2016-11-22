# Makefile

.DEFAULT:

TARGET = 2nd_diff BaseAngle EMstepmake RMS beam calc_SD corr_stage distortion-i distortion distortionVTX inverseVTX rangeCut reducedVTX rotation splitVTX step zerobeam
TARGETCOPY = conv_vtx.sh shrink.awk trk2d trk2d.C vtx2trk.sh

INSTALL = install -c -m 775
INSTALL_DIR = install -d -m 775

PREFIX?= ../
BINDIR?= $(PREFIX)/bin
ROOTBINDIR?= $(PREFIX)/rootfile

CC := g++
CXX := g++
CXXFLAGS += -fopenmp
ROOTFLAGS = `root-config --cflags`
ROOTLIBS = `root-config --libs`

all: bin root
bin: $(TARGET)

%: %.C
%.o: %.C

.c.o:
	$(CXX) -c $(CXXFLAGS) $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<

.cc.s:
	$(CXX) -S $(CXXFLAGS) $<

.o:
	$(CXX) $(CXXFLAGS) $< -o $@ $^

-include include.dep

install: $(TARGET) $(TARGETCOPY)
	$(INSTALL_DIR) $(BINDIR)/
	$(INSTALL_DIR) $(ROOTBINDIR)/
	$(INSTALL) $(TARGET) $(TARGETCOPY) $(BINDIR)/

clean:
	$(RM) *.o *.O core.*

distclean:
	$(RM) *.o *.O core.* $(TARGET)










