#include "mico.h"

mico_logic_partition_t* MicoFlashGetInfo( mico_partition_t inPartition )
{
    return mhal_flash_get_info(inPartition);
}

OSStatus MicoFlashErase(mico_partition_t inPartition, uint32_t off_set, uint32_t size)
{
    return mhal_flash_erase(inPartition, off_set, size);
}

OSStatus MicoFlashWrite( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* inBuffer ,uint32_t inBufferLength)
{
    return mhal_flash_write(inPartition, off_set, inBuffer, inBufferLength);
}

OSStatus MicoFlashRead( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* outBuffer, uint32_t inBufferLength)
{
    return mhal_flash_read(inPartition, off_set, outBuffer, inBufferLength);
}


