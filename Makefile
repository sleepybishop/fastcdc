OBJS=\
fastcdc.o

CPPFLAGS :=
CFLAGS := -g -O3 -std=c99 -Wall 
LDFLAGS := -lm  

all: test

test: $(OBJS) 

.PHONY: clean indent scan 
clean:
	$(RM) *.o *.a

indent:
	clang-format -i *.h *.c

