#ifndef __MY_BUDDY_ALLOC_H__
#define __MY_BUDDY_ALLOC_H__

#include <stdlib.h>

/* Represents true-or-false values */
typedef int bool;

/* Explicitly-sized versions of integer types */
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

// 定义结构体名
struct buddy;

// init
struct buddy* buddy_init(uint32_t size);

// destroy
void buddy_destroy(struct buddy* self);

// alloc
uint32_t buddy_alloc(struct buddy* self, uint32_t size);

// free
void buddy_free(struct buddy* self, uint32_t offset);

uint32_t buddy_size(struct buddy* self, uint32_t offset);
void buddy_dump(struct buddy* self);

#endif







