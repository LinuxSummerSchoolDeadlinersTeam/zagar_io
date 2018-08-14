all : directories build/example_main.o build/example.o build/client
	gcc -o example build/example_main.o build/example.o
build/example_main.o : src/example_main.c
	gcc -o build/example_main.o -c src/example_main.c
build/example.o : src/example.c
	gcc -o build/example.o -c src/example.c
build/client : src/client.c
	g++ src/client.c -lSDL2 -lSDL2_image -o build/client
directories :
	mkdir -p build
clean : 
	rm -rf example build/*
doc :
	doxygen doxygen/doxyconf
cleandoc :
	rm -rf doxygen/html
