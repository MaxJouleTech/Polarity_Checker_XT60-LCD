/*
 * syscalls.c - Minimal newlib system call stubs
 *
 * Replace --specs=nosys.specs (which links stubs that print linker warnings).
 * _write() will later send printf() output to the debug UART.
 */
#include <errno.h>
#include <sys/stat.h>

extern char _heap_start;    /* from the linker script */
extern char _heap_end;

int _write(int fd, const char *buf, int len)
{
    (void)fd; (void)buf;
    return len;             /* no output yet: discard */
}

int _read(int fd, char *buf, int len)  { (void)fd; (void)buf; (void)len; errno = EAGAIN; return -1; }
int _close(int fd)                     { (void)fd; return -1; }
int _lseek(int fd, int pos, int dir)   { (void)fd; (void)pos; (void)dir; return 0; }
int _fstat(int fd, struct stat *st)    { (void)fd; st->st_mode = S_IFCHR; return 0; }
int _isatty(int fd)                    { (void)fd; return 1; }
int _getpid(void)                      { return 1; }
int _kill(int pid, int sig)            { (void)pid; (void)sig; errno = EINVAL; return -1; }
void _exit(int status)                 { (void)status; for (;;) { } }

void *_sbrk(int incr)
{
    static char *brk = &_heap_start;
    char *prev = brk;

    if ((brk + incr) > &_heap_end) {
        errno = ENOMEM;
        return (void *)-1;
    }
    brk += incr;
    return prev;
}
