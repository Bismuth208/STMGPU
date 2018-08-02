#ifndef _MEMHELPER_H
#define _MEMHELPER_H

#include <stdint.h>

void memcpy8(void * dst, void const * src, size_t len);
void memcpy32(void *dst, void const *src, size_t len);

void memset32(void *dst, uint32_t src, size_t len);

void init_DMA_memset(void);
void memset_DMA1(void *dst, void const *src, size_t len);

#endif /* _MEMHELPER_H */
