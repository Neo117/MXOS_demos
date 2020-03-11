#include <string.h>
#include <stdio.h>
#include "mos.h"

uint32_t cpu_intrpt_save(void);
void cpu_intrpt_restore(uint32_t cpsr);

#define CPSR_ALLOC() uint32_t cpsr
#define RHINO_CPU_INTRPT_DISABLE() \
    do                             \
    {                              \
        cpsr = cpu_intrpt_save();  \
    } while (0)
#define RHINO_CPU_INTRPT_ENABLE() \
    do                            \
    {                             \
        cpu_intrpt_restore(cpsr); \
    } while (0)

void *aos_malloc(unsigned int size);
void aos_free(void *p);

// NOTE: heap functions
// -----------------------------------------------------------------------------------------------------------------------------
#define RECORD_LIST_NUM 200

#define HEAD_GUARD 0xDEADDEAD
#define TAIL_GUARD 0xBEEFBEEF

static mos_mallrecord_t record_list[RECORD_LIST_NUM];
static int record_index;

void mos_mallrecord_show(void)
{
    uint32_t *pv;
    uint32_t word_sz;
    printf("#\tthread id\tthread name\tcaller address\tmemory address\tmemory size\thead guard\ttail guard\t\r\n");
    printf("------------------------------------------------------------------------------------------------------------------\r\n");
    for (int i = 0; i < record_index; i++)
    {
        pv = (uint32_t *)record_list[i].addr - 1;
        word_sz = ((record_list[i].size + 3) >> 2) + 2;
        printf("%d\t%p\t%-15s\t%p\t0x%lx\t0x%-8lx\t0x%lx\t0x%lx\r\n",
               i,
               record_list[i].thread,
               mos_thread_get_name(record_list[i].thread),
               record_list[i].caller,
               record_list[i].addr,
               record_list[i].size,
               pv[0],
               pv[word_sz - 1]);
    }
}

/*
-------------------------------------------------
| 1 word     | N words             | 1 word     |
|------------+---------------------+------------|
| HEAD_GUARD | user memory | align | TAIL_GUARD |
-------------------------------------------------
*/

void *__wrap__malloc_r(void *p, size_t size)
{
    uint32_t *pv;
    size_t word_sz;

    word_sz = ((size + 3) >> 2) + 2;
    if ((pv = aos_malloc(word_sz * 4)) == NULL)
    {
        return NULL;
    }

    pv[0] = HEAD_GUARD;
    pv[word_sz - 1] = TAIL_GUARD;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    if (record_index < RECORD_LIST_NUM)
    {
        record_list[record_index].thread = mos_thread_get_id();
        record_list[record_index].caller = __builtin_return_address(0);
        record_list[record_index].size = size;
        record_list[record_index].addr = (uint32_t)&pv[1];
        record_index++;
    }
    RHINO_CPU_INTRPT_ENABLE();

    return &pv[1];
}

void __wrap__free_r(void *p, void *x)
{
    size_t word_sz;
    uint32_t *pv = (uint32_t *)x;

    if (pv == NULL)
        return;

    pv--;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    for (int i = 0; i < record_index; i++)
    {
        if (record_list[i].addr == (uint32_t)x)
        {
            word_sz = ((record_list[i].size + 3) >> 2) + 2;
            if (pv[0] != HEAD_GUARD || pv[word_sz - 1] != TAIL_GUARD)
            {
                printf("WARNING! MEMORY CORRUPTED!\r\n");
                printf("#\tthread id\tthread name\tcaller address\tmemory address\tmemory size\thead guard\ttail guard\t\r\n");
                printf("------------------------------------------------------------------------------------------------------------------\r\n");
                printf("0\t%p\t%-15s\t%p\t0x%lx\t0x%-8lx\t0x%lx\t0x%lx\r\n",
                       record_list[i].thread,
                       mos_thread_get_name(record_list[i].thread),
                       record_list[i].caller,
                       record_list[i].addr,
                       record_list[i].size,
                       pv[0],
                       pv[word_sz - 1]);
            }
            record_index--;
            memcpy(&record_list[i], &record_list[record_index], sizeof(mos_mallrecord_t));
            break;
        }
    }
    RHINO_CPU_INTRPT_ENABLE();

    aos_free(pv);
}

void *__wrap__calloc_r(void *p, size_t a, size_t b)
{
    void *pv;
    size_t sz = a * b;

    if ((pv = __wrap__malloc_r(p, sz)) == NULL)
    {
        return NULL;
    }
    memset(pv, 0, sz);

    return pv;
}

void *__wrap__realloc_r(void *p, void *x, size_t sz)
{
    void *pv;

    if ((pv = __wrap__malloc_r(p, sz)) == NULL)
    {
        return NULL;
    }
    memcpy(pv, x, sz);
    __wrap__free_r(p, x);

    return pv;
}
