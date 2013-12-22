all: configure compile

configure:
	./waf configure

compile:
	./waf build

clean:
	./waf clean
