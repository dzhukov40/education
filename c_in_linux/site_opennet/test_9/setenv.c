/* setenv.c */

// http://www.opennet.ru/docs/RUS/zlp/index.html
// информация [info.sh]

/*
  В заголовочном файле [stdlib.h] объявлена функция [setenv]
  вотпрототип ф-ии [int setenv (const char * name, const char * value, int overwrite);]

  Функция [setenv()] устанавливает значение (второй аргумент, value) для 
  переменной окружения (первый аргумент, name). Третий аргумент - это флаг 
  перезаписи. При ненулевом флаге уже существующая переменная перезаписывается, 
  при нулевом флаге переменная, если уже существует, - не перезаписывается. 
  В случае успешного завершения [setenv()] возвращает нуль (даже если существующая
  переменная не перезаписалась при overwrite==0). Если в окружении нет места для 
  новой переменной, то [setenv()] возвращает -1.   
*/

#include <stdio.h>
#include <stdlib.h>

#define FL_OVWR		1	/* чето не пашет!!! как ожидалось */

int main (int argc, char ** argv)
{	
	if (argc < 3)
	{
		fprintf (stderr, "setenv: Too few arguments\n");
		fprintf (stderr, 
			"Usage: setenv <variable> <value>\n");
		exit (1);
	}
	if (setenv (argv[1], argv[2], FL_OVWR) != 0)
	{
		fprintf (stderr, "setenv: Cannot set '%s'\n", argv[1]);
	       	exit (1);
	}

	printf ("%s=%s\n", argv[1], getenv (argv[1]));	
	exit (0);
}
