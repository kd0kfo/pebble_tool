SRCDIR=../src

all: test

test: encode decode
	./encode Test Message > encode.test
	./decode ... --- ... > decode.test
	diff encode.canonical encode.test
	diff decode.canonical decode.test

decode: decode.o depends
	gcc -o decode decode.o morse.o  -g -O0

encode: encode.o depends
	gcc -o encode encode.o morse.o  -g -O0

clean:
	rm -f *.o decode encode

depends:
	gcc -o morse.o -I $(SRCDIR) -c $(SRCDIR)/morse.c  -g -O0

%.o: %.c
	gcc -o $@ -c $< -I $(SRCDIR) -g -O0