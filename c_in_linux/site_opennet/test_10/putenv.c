/* putenv.c */

// http://www.opennet.ru/docs/RUS/zlp/index.html
// информация [info.sh]

/*
  В заголовочном файле [stdlib.h] объявлена функция [putenv]
  вотпрототип ф-ии [int putenv (char * str);]

   Функция [putenv()], объявленная в заголовочном файле [stdlib.h] вызывается 
   с единственным аргументом - строкой формата [ПЕРЕМЕННАЯ=ЗНАЧЕНИЕ] или 
   просто [ПЕРЕМЕННАЯ]. Обычно такие преформатированные строки называют 
   запросами. Если переменная отсутствует, то в окружение добавляется 
   новая запись. Если переменная уже существует, то текущее значение 
   перезаписывается. Если в качестве аргумента фигурирует просто имя 
   переменной, то переменная удаляется из окружения. В случае удачного 
   завершения, [putenv()] возвращает [нуль] и [-1] - в случае ошибки.

   У функции [putenv()] есть одна особенность: указатель на строку, переданный  
   в качестве аргумента, становится частью окружения. Если в дальнейшем строка 
   будет изменена, будет изменено и окружение. Это очень важный момент, о котором 
   не следует забывать. Ниже приведен адаптированный прототип функции [putenv]  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUERY_MAX_SIZE		32
char * query_str;

void print_evar (const char * var)
{
	char * tmp = getenv (var);
	if (tmp == NULL)
	{
		printf ("%s is not set\n", var);
		return;
	}
	printf ("%s=%s\n", var, tmp);
}

int main (void)
{
	int ret;
	query_str = (char *) calloc (QUERY_MAX_SIZE, sizeof(char));
	if (query_str == NULL) abort ();

	strncpy (query_str, "FOO=foo_value1", QUERY_MAX_SIZE-1);
	ret = putenv (query_str);
	if (ret != 0) abort ();
	print_evar ("FOO");

	strncpy (query_str, "FOO=foo_value2", QUERY_MAX_SIZE-1);
	print_evar ("FOO");

	strncpy (query_str, "FOO", QUERY_MAX_SIZE-1);
	ret = putenv (query_str);
	if (ret != 0) abort ();
	print_evar ("FOO");
	
	free (query_str);
	exit (0);
}
