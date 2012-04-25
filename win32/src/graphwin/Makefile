ROOT        =../../..
TARGET      =libgraphwin.a
SRCS        =$(wildcard *.cpp)
OBJS        =$(SRCS:.cpp=.o)
INSTALL_DIR ?=$(ROOT)/build

CPPFLAGS    += -Wall -I$(ROOT)/core/include/geom \
               -I$(ROOT)/core/include/graph \
               -I$(ROOT)/win32/include/graphwin

all:        $(TARGET)
$(TARGET):  $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	rm -rf *.o *.a
ifdef touch
	touch -c *
endif

install:
	@test -d $(INSTALL_DIR) || mkdir $(INSTALL_DIR)
	@! test -e $(TARGET) || cp -v $(TARGET) $(INSTALL_DIR)
