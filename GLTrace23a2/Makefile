CFLAGS+=-Wall -shared -g

libGL.so.1: libGL.so
	ln -sf $< $@

libGL.so: gltrace.o glwrapper.o font.o input.o log.o dll.o
	gcc -shared -o $@ $^ -ldl

clean:
	rm -f *~ *.o
