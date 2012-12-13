TARGET = fucload

all: $(TARGET)
libpg.o: libpg.c libio.h libpg.h
	c99 $< -o $@ -c

libio.o: libio.c libio.h
	c99 $< -o $@ -c

%: %.c libpg.o libio.o libpg.h libio.h
	c99 $< libpg.o libio.o -o $@ -g -Wall -lpciaccess

clean:
	rm -f $(TARGET) *.o *~
