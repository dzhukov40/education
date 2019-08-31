/* getpid.c */

// http://www.opennet.ru/docs/RUS/zlp/index.html
// информация [info.sh]

/*
  Для создания дочернего прочесса
  [pid_t fork (void);] - после вызова дальше программу будут 
  выполнять два процесса!!!
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main (void)
{
        pid_t pid = fork (); /* начитая с этой точки [pid_t pid = fork ();] 
                                дальше выполняется код двумя прочессами
                                определить кто есть кто помогает [pid]
                                для [child] он [0] 
                                для [parent] получит индификатор [child]
                              */
        if (pid == 0) {
                printf ("child (pid=%d)\n", getpid());
        } else {
                printf ("parent (pid=%d, child's pid=%d)\n", getpid(), pid);
        }

        return 0;
}

