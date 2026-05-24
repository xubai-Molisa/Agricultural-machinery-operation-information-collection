#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "stm32f4xx_hal.h"

extern char _end;
extern uint32_t _estack;

static char *heap_end;

int _write(int file, char *ptr, int len)
{
    (void)file;

    if ((USART1->CR1 & USART_CR1_UE) == 0U) {
        return len;
    }

    for (int i = 0; i < len; ++i) {
        uint32_t timeout = 100000U;
        while (((USART1->SR & USART_SR_TXE) == 0U) && (timeout > 0U)) {
            --timeout;
        }
        if (timeout == 0U) {
            break;
        }
        USART1->DR = (uint8_t)ptr[i];
    }

    return len;
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int _close(int file)
{
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void)
{
    return 1;
}

caddr_t _sbrk(int incr)
{
    if (heap_end == 0) {
        heap_end = &_end;
    }

    char *prev_heap_end = heap_end;
    char *next_heap_end = heap_end + incr;
    char *stack_limit = (char *)&_estack;

    if (next_heap_end >= stack_limit) {
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end = next_heap_end;
    return (caddr_t)prev_heap_end;
}
