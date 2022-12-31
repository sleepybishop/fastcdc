OBJS=\
fastcdc.o

CPPFLAGS := -D_DEFAULT_SOURCE
CFLAGS := -g -O3 -std=c99 -Wall 
LDLIBS := -lm

all: test

test: $(OBJS) 

.PHONY: clean indent scan 
clean:
	$(RM) *.o *.a

indent:
	clang-format -i *.h *.c

