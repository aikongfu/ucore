#include <stdio.h>
#include <ulib.h>

int
main(void) {
    int pid, exit_code;
    if ((pid = fork()) == 0) {
        cprintf("fork ok.\n");
        int i;
        for (i = 0; i < 10; i ++) {
            cprintf("yeild begin [%d].\n", i);
            yield();
            cprintf("yeild end [%d].\n", i);
        }
        cprintf("exit begin.\n", i);
        exit(0xbeaf);
        cprintf("exit end.\n", i);
    }
    cprintf("assert(pid > 0);\n");
    assert(pid > 0);
    cprintf("assert(waitpid(-1, NULL) != 0);\n");
    assert(waitpid(-1, NULL) != 0);
    cprintf("assert(waitpid(pid, (void *)0xC0000000) != 0);\n");
    assert(waitpid(pid, (void *)0xC0000000) != 0);
    cprintf("assert(waitpid(pid, &exit_code) == 0 && exit_code == 0xbeaf);\n");
    assert(waitpid(pid, &exit_code) == 0 && exit_code == 0xbeaf);
    cprintf("badarg pass.\n");
    return 0;
}

