ROOT        =../..
TARGET      =libgeom.a
SRCS        =$(wildcard *.cpp)
OBJS        =$(SRCS:.cpp=.o)
INSTALL_DIR ?=$(ROOT)/build

CPPFLAGS    += -I$(ROOT)/include/geom

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
	@cp -v $(TARGET) $(INSTALL_DIR)
