#!/bin/bash

msg1="http://www.opennet.ru/docs/RUS/zlp/index.html
До этого мы просто дали компилятору файл .c
и получили на выходе бинарник
-> выполнив [gcc -o hello hello.c]"

msg2="у нас есть два файла [main.c и hello.c]
хотим сделать обьектные файлы этих исходников.
-> выполняем [gcc -c main.c] и [gcc -c hello.c].
Теперь обьединяем их в бинарник
-> выполняем [gcc -o hello main.o  hello.o]"

echo "$msg1"
echo ""
echo "$msg2"
