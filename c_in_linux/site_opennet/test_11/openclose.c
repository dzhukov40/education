/* openclose.c.c */

// http://www.opennet.ru/docs/RUS/zlp/index.html
// информация [info.sh]

/*
   Чтобы получить возможность прочитать что-то из файла или записать что-то 
   в файл, его нужно открыть. Это делает системный вызов [open()].
   [int open (const char * filename, int flags, ...);]

   Системный вызов close() закрывает файл. Вообще говоря, по завершении 
   процесса все открытые файлы (кроме файлов с дескрипторами 0, 1 и 2) 
   автоматически закрываются.
   [int close (int fd);]
*/

#include <fcntl.h>	/* open() and O_XXX flags */
#include <sys/stat.h>	/* S_IXXX flags */
#include <sys/types.h>	/* mode_t */
#include <unistd.h>	/* close() */
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char ** argv)
{
	int fd;
	mode_t mode = S_IRUSR | S_IWUSR;
	int flags = O_WRONLY | O_CREAT | O_EXCL;
	if (argc < 2)
	{
		fprintf (stderr, "openclose: Too few arguments\n");
		fprintf (stderr, "Usage: openclose <filename>\n");
		exit (1);
	}

	fd = open (argv[1], flags, mode);
	if (fd < 0)
	{
		fprintf (stderr, "openclose: Cannot open file '%s'\n",
				argv[1]);
		exit (1);
	}
	
	if (close (fd) != 0)
	{
		fprintf (stderr, "Cannot close file (descriptor=%d)\n", fd);
		exit (1);
	}	
	exit (0);
}
