/*
 * author   : snow yang
 * date     : 2019-07-17
 */

#include "mxos.h"

void LOGUART_PutChar(uint8_t c);

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    mhal_uart_write(MXOS_LOG_UART, buf, nbytes);
    
    return nbytes;
}