all:
	cp -r scc/release bin
	bin/scpp build.c bin/build.i
	bin/scc bin/build.i bin/build.bcode
	bin/bcode bin/build.bcode bin/build.asm
	bin/asm bin/build.asm bin/build
	bin/build
clean:
	rm -rf bin build build.log
