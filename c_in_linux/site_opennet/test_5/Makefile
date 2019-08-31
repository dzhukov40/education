# Makefile for Hello World project

# Суть в том что при использовании этого файла 
# без ключей по умолчанию будет решаться первая цель
# тоесть получение [hello] из [main.o] и [hello.o]

# но у нас нет этих файлов. изначально у нас только
# два .c файла. поэтому ниже мы описываем 
# {что получится}: {что нужно (файл должен быть)}
# \t {команда которую надо выполнить}


binary: main.o libworld.a
	gcc -o binary main.o -L. -lworld

main.o: main.c
	gcc -c main.c

libworld.a: h_world.o g_world.o
	ar cr libworld.a h_world.o g_world.o

h_world.o: h_world.c
	gcc -c h_world.c

g_world.o: g_world.c
	gcc -c g_world.c

djoClean:
	rm -f *.o *.a binary
