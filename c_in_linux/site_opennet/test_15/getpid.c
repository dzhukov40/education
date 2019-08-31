/* getpid.c */

// http://www.opennet.ru/docs/RUS/zlp/index.html
// информация [info.sh]

/*
  Процесс может узнать свой идентификатор (PID), а также 
  родительский идентификатор (PPID) при помощи системных 
  вызовов [getpid()] и [getppid()]. 
  Для использования [getpid()] и [getppid()] в программу 
  должны быть включены директивой [#include] заголовочные 
  файлы [unistd.h] и [sys/types.h] (для типа [pid_t]).
  [pid_t getpid (void);]
  [pid_t getppid (void);]
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main (void)
{
        pid_t pid, ppid;

        pid = getpid ();
        ppid = getppid ();

        printf ("PID: %d\n", pid);
        printf ("PPID: %d\n", ppid);

        fprintf (stderr, "Press <Enter> to exit...");
        getchar ();
        return 0;
}

