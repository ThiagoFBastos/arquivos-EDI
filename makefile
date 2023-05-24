FILES = NTree.o Date.o Directory.o File.o List.o cstring.o explorer.o
compile: $(FILES)
	gcc -o explorer $(FILES)
NTree.o: NTree.c NTree.h
	gcc -I . -c NTree.c -O3
Date.o: Date.c Date.h
	gcc -I . -c Date.c -O3
List.o: List.c List.h NTree.h
	gcc -I . -c List.c -O3
File.o: File.c File.h Date.h cstring.h
	gcc -I . -c File.c -O3
Directory.o: Directory.c Directory.h File.h
	gcc -I . -c Directory.c -O3
cstring.o: cstring.c cstring.h
	gcc -I . -c cstring.c -O3
explorer.o: explorer.c NTree.h Directory.h List.h
	gcc -I . -c explorer.c -O3
clean:
	rm -rf *.o
	rm explorer
