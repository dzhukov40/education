# Makefile for Hello World project

# Суть в том что при использовании этого файла 
# без ключей по умолчанию будет решаться первая цель
# тоесть получение [hello] из [main.o] и [hello.o]

# но у нас нет этих файлов. изначально у нас только
# два .c файла. поэтому ниже мы описываем 
# {что получится}: {что нужно (файл должен быть)}
# \t {команда которую надо выполнить}


binary: main.o libworld.so
	gcc -o binary main.o -L. -lworld -Wl,-rpath,.

main.o: main.c
	gcc -c main.c

libworld.so: h_world.o g_world.o
	gcc -shared -o libworld.so h_world.o g_world.o

h_world.o: h_world.c
	gcc -c -fPIC h_world.c

g_world.o: g_world.c
	gcc -c -fPIC g_world.c

djoClean:
	rm -f *.o *.so binary
