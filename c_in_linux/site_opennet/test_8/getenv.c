/* getenv.c */

// http://www.opennet.ru/docs/RUS/zlp/index.html
// информация [info.sh]

/*
  В заголовочном файле [stdlib.h] объявлена функция [getenv]
  вотпрототип ф-ии [char * getenv (const char * name);]

  Функция эта работает очень просто: если в качестве аргумента 
  указано имя существующей переменной окружения, то функция 
  возвращает указатель на строку, содержащую значение этой переменной; 
  если переменная отсутствует, возвращается NULL.   
*/

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv)
{
	if (argc < 2)
	{
		fprintf (stderr, "getenv: Too few arguments\n");
		fprintf (stderr, "Usage: getenv <variable>\n");
		exit (1);
	}
	char * var = getenv (argv[1]); // просто используем библиотечную ф-ию!
	if (var == NULL)
	{
		printf ("'%s' not found\n", argv[1]);
		exit (0);
	}
	printf ("'%s=%s' found\n", argv[1], var);
	exit (0);
}
