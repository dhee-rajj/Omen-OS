#ifndef KHEAP_H
#define KHEAP_H
#include <stdint.h>
#include <stddef.h>

void kheap_init();
void* kheap_malloc(size_t size);
void kfree(void* ptr);

#endif