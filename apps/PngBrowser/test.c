#include <sys/stat.h>
#include <errno.h>

extern int errno;

int _close(int file) {
    errno = EBADF;
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    errno = EBADF;
    return -1;
}

int _read(int file, char *ptr, int len) {
    errno = EBADF;
    return -1;
}

int _write(int file, char *ptr, int len) {
    errno = EBADF;
    return -1;
}