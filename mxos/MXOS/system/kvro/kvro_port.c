#include "mxos.h"

int kvro_flash_read(uint32_t offset, void *buf, uint32_t nbytes)
{
    return mhal_flash_read(MODULE_PARTITION_KVRO, &offset, buf, nbytes);
}

int kvro_flash_write(uint32_t offset, void *buf, uint32_t nbytes)
{
    return mhal_flash_write(MODULE_PARTITION_KVRO, &offset, buf, nbytes);
}

void *kvro_malloc(uint32_t size)
{
    return malloc(size);
}

void kvro_free(void *ptr)
{
    free(ptr);
}