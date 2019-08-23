# Makefile for vips plugin 

# Object files
OBJS = gmic.o vips_gmic.o

# Source files
SRC = $(BASEOBJS:.o=.cpp) 

# Headers
HEADERS = Cimg.h gmic_def.h gmic.h 

# All
TAGS = $(SRC) $(HEADERS)

# gmic/cimg config
# dont use the threading stuff, vips does that

# linux
CXXFLAGS = -Dgmic_build -Dcimg_strict -Dcimg_OS=1 -Dcimg_display=0 
# win32
#CXXFLAGS = -Dgmic_build -Dcimg_strict -Dcimg_OS=2 -Dcimg_display=0 

#linux
CXXFLAGS += -shared -fPIC 
#solaris
#CXXFLAGS += -Xa -Kpic 
CPPFLAGS += `pkg-config vips-cpp --cflags`
LDFLAGS += `pkg-config vips-cpp --libs`
OUT = vips-gmic.plg

release: $(OUT)
debug: $(OUT)

# don't use -Wall ... G'MIC will drown you
.PHONY: debug  
debug: CXXFLAGS += -g 

.PHONY: release  
release: CXXFLAGS += -O3

$(OUT): $(OBJS)
#linux ... must link C++ plugins with the C++ compiler
	$(CXX) -o $(OUT) -shared $(OBJS)
#solaris
#	ld -o $(OUT) -G $(OBJS)
#	vips -plugin ./$(OUT) resample in.v in2.v out.v 0.1 20 1 1 0
#	vips -plugin ./$(OUT) scale_par matrix.mat out.mat 2.0

clean: 
	$(RM) $(OBJS) 

tags: $(TAGS)
	ctags $(TAGS)

# version as MAJOR.MINOR
VIPS_VERSION = $(shell pkg-config vips-cpp --modversion | \
	         awk '{split($$1,a,"."); print a[1]"."a[2]}')
PLUGIN_DIR = $(VIPSHOME)/lib/vips-plugins-$(VIPS_VERSION)

install: $(OUT)
	-mkdir -p $(PLUGIN_DIR)
	-cp $(OUT) $(PLUGIN_DIR)

