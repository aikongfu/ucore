#include "buddy.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define LEFT_LEAF(index) ((index) * 2 + 1)
#define RIGHT_LEAF(index) ((index) * 2 + 2)

#define PARENT(index) (((index) + 1 ) / 2 - 1)

#define IS_POWER_OF_2(x) (!((x) & ((x) - 1)))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define ALLOC malloc
#define FREE free

// 找到大于等于size的最小的2的幂数
// 3 -> 4;  15 -> 16;  211 -> 256; 1000 -> 1024
static uint32_t fixsize(uint32_t size) {
    size -= 1;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size + 1;
}

// 定义结构体名
struct buddy {
    uint32_t size;
    uint32_t longest[1];
} __attribute__((packed));

// init
struct buddy* buddy_init(uint32_t size) {
    struct buddy *self;
    uint32_t node_size;
    int i;

    if (size < 1 || !IS_POWER_OF_2(size)) {
        return NULL;
    }

    // 给存储buddy 分配空间
    self = ALLOC(size * 2 * (sizeof(uint32_t)));
    self->size = size;
    node_size = size * 2;

    for (int i = 0; i < 2 * size; i++) {
        if (IS_POWER_OF_2(i + 1)) {
            node_size /= 2;
        }
        self->longest[i] = node_size;
    }

    return self;
}

// destroy
void buddy_destroy(struct buddy* self) {
    FREE(self);
}

// alloc
uint32_t buddy_alloc(struct buddy* self, uint32_t size) {
    uint32_t index = 0;
    uint32_t node_size;
    uint32_t offset = 0;
    uint32_t left;
    uint32_t right;
    if (self == NULL) {
        return -1;
    }
    if (size <= 0) {
        size = 1;
    } else if (!IS_POWER_OF_2(size)){
        size = fixsize(size);
    }

    // 无足够空间
    if (self->longest[index] < size) {
        return -1;
    }

    // 开始查找符合条件的节点
    for (node_size = self->size; node_size != size; node_size /= 2) {
        left = LEFT_LEAF(index);
        right = RIGHT_LEAF(index);

        // 如果两个子节点都大于size，取较小的那个
        if (self->longest[left] >= size && self->longest[right] >= size) {
            index = self->longest[left] >= self->longest[right] ? left : right;
            continue;
        }
        // 如果一个小于，取另一个
        if (self->longest[left] >= size) {
            index = left;
        } else if (self->longest[right] >= size){
            index = right;
        }
    }

    self->longest[index] = 0;
    offset = (index + 1) * node_size - self->size;

    // 回溯，更新父节点可分配的大小
    while (index) {
        index = PARENT(index);
        self->longest[index] = 
        MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
    }
    
    return offset;
}

// free
void buddy_free(struct buddy* self, uint32_t offset) {
    uint32_t node_size, index = 0;
    uint32_t left_longest, right_longest;

    assert(self && offset >=0 && offset < self->size);

    node_size = 1;
    index = offset + self->size - 1;

    for (; self->longest[index]; index = PARENT(index)) {
        node_size *= 2;
        if (0 == index) {
            return;
        }
    }

    self->longest[index] = node_size;
    while (index) {
        index = PARENT(index);
        node_size *= 2;

        left_longest = self->longest[LEFT_LEAF(index)];
        right_longest = self->longest[RIGHT_LEAF(index)];

        if (left_longest + right_longest == node_size) {
            self->longest[index] == node_size;
        } else {
            self->longest[index] = MAX(left_longest, right_longest);
        }
        
    }

}

uint32_t buddy_size(struct buddy* self, uint32_t offset) {
    uint32_t node_size, index = 0;
    assert(self && offset >= 0 && offset < self->size);

    node_size = 1;
    for (index = offset + self->size - 1; self->longest[index]; index = PARENT(index)) {
        node_size *= 2;
    }

    return node_size;
}


void buddy_dump(struct buddy* self) {
    char canvas[65];
    int i, j;
    uint32_t node_size, offset;

    if (self == NULL) {
        printf("buddy_dump: (struct buddy*)self == NULL");
        return;
    }

    if (self->size > 64) {
        printf("buddy_dump: (struct buddy*)self is too big to dump");
        return;
    }

    memset(canvas, '_', sizeof(canvas));

    node_size = self->size * 2;
    for (int i = 0; i < 2 * self->size * 2; ++i) {
        if (IS_POWER_OF_2(i)) {
            node_size /= 2;
        }

        if (self->longest[i] == 0) {
            if (i >= self->size - 1) {
                canvas[i - self->size + 1] = '*';
            } else if (self->longest[LEFT_LEAF(i)] && self->longest[RIGHT_LEAF(i)]) {
                offset = (i + 1) * node_size - self->size;
                for (j = offset; j < offset + node_size; ++j) {
                    canvas[j] = '*';
                }
            }

        }
    }

    canvas[self->size] = '\0';
    puts(canvas);
}
