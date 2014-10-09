# Makefile for vips plugin 

# Object files
OBJS = gmic.o vips_gmic.o

# Source files
SRC = $(BASEOBJS:.o=.cpp) 

# Headers
HEADERS = Cimg.h gmic_def.h gmic.h 

# All
TAGS = $(SRC) $(HEADERS)

# linux
CXXFLAGS = -Dgmic_build -Dcimg_strict -Dcimg_OS=1 -Dcimg_display_type=0 
# win32
#CXXFLAGS = -Dgmic_build -Dcimg_strict -Dcimg_OS=2 -Dcimg_display_type=0 

#linux
CXXFLAGS += -shared -fPIC 
#solaris
#CXXFLAGS += -Xa -Kpic 
CPPFLAGS += `pkg-config vipsCC --cflags`
LDFLAGS += `pkg-config vipsCC --libs`
OUT = vips-gmic.plg

all: $(OUT)
debug: $(OUT)

all := CXXFLAGS += -O
debug := CXXFLAGS += -g
sb := CXXFLAGS += -g 

$(OUT): $(OBJS)
#linux
	$(CXX) -o $(OUT) -shared $(OBJS)
#solaris
#	ld -o $(OUT) -G $(OBJS)
#	vips -plugin ./$(OUT) resample in.v in2.v out.v 0.1 20 1 1 0
#	vips -plugin ./$(OUT) scale_par matrix.mat out.mat 2.0

clean: 
	$(RM) $(OBJS) 

tags: $(TAGS)
	ctags $(TAGS)
