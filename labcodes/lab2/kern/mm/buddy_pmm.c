#include <pmm.h>
#include <defs.h>
#include <list.h>
#include <string.h>
#include <buddy_pmm.h>
#include <stdio.h>


#define LEFT_LEAF(index) ((index) * 2 + 1)
#define LEFT_RIGHT(index) ((index) * 2 + 2)
#define PARENT(index) (((index) + 1 ) / 2 - 1)

#define IS_POWER_OF_2(n) (!((n) & ((n) - 1)))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define UINT32_T_OR_RIGHT_MOVE(x, n) ((x) | ((x) >> (n)))

#define UINT32_MASK(a) (UINT32_SHR_OR(UINT32_SHR_OR(UINT32_SHR_OR(UINT32_SHR_OR(UINT32_SHR_OR(a,1),2),4),8),16))    

//大于a的一个最小的2^k
#define UINT32_REMAINDER(a)     ((a)&(UINT32_MASK(a)>>1))

//小于a的最大的2^k
#define UINT32_ROUND_DOWN(a)    (UINT32_REMAINDER(a)?((a)-UINT32_REMAINDER(a)):(a))


free_area_t free_area;

#define free_list (free_area.free_list)
#define nr_free (free_area.nr_free)

struct buddy {
    uint32_t size;  // 内存大小
    uint32_t longest[1024 * 1024 - 1];
};
// 存入buddy信息，用来分配内存
struct buddy *root;

// 记录分配块的信息
struct allocRecord {
    struct Page *base;
    uint32_t offset;
    size_t nr;      // 块大小
};

// 存放偏移量的数组
struct allocRecord *alloced[1024 * 1024];
// 已分配的块数
uint32_t nr_block;

//找到大于等于所需内存的2的倍数
static uint32_t uint32_round_up(uint32_t size) {
  size -= 1;
  size |= size >> 1;
  size |= size >> 2;
  size |= size >> 4;
  size |= size >> 8;
  size |= size >> 16;
  return size+1;
}

static uint32_t uint32_round_down(uint32_t size) {
  return size & (uint32_round_up(size) >> 1);
}

void buddy_new(uint32_t size) {
    uint32_t node_size;
    uint32_t i;
    nr_block = 0;

    // check
    if (size < 1 || !IS_POWER_OF_2(size)) {
        return;
    }

    node_size = 2 * size;
    root->size = size;

    for (i = 0; i < 2 * size - 1; i++) {
        if (IS_POWER_OF_2(i + 1)) {
            node_size /= 2;
        }

        root->longest[i] = node_size;
    }

}

static uint32_t
buddy_alloc(struct buddy *self, uint32_t size) {
    uint32_t index;
    uint32_t node_size;
    uint32_t offset;

    if (self == NULL) {
        return -1;
    }

    if (size <= 0) {
        size = 1;
    } else if (!IS_POWER_OF_2(size)) {
        size = uint32_round_up(size);
    }

    return NULL;
}


static void
buddy_init(void) {
    list_init(&free_list);
    nr_free=0;
}

static void
buddy_init_memmap(struct Page *base, size_t n) {
    assert(n > 0);
    struct Page *p = base;

    for (; p != base + n; p++) {
        assert(PageReserved(p));
        p->flags = 0;
        p->property = 0;
        set_page_ref(p, 0);
    }

    SetPageProperty(base);
    base->property = n;
    nr_free += n;

    // 这里只是简化处理，这样存在一个问题，真正可用的页数就是小于等于n的最大的一个2^k次幂，所以size - n 将浪费掉，也可以继续分割（size - n）
    uint32_t size = uint32_round_down(n);
    buddy_new(size);
}


static struct Page *
buddy_alloc_pages(size_t n) {
    uint32_t index;
    uint32_t node_size;
    uint32_t offset;

    return NULL;
}


static void
buddy_free_pages(struct Page *base, size_t n) {
 
}

static size_t
buddy_nr_free_pages(void) {

}

static void 
buddy_check(void) {

}


const struct pmm_manager buddy_pmm_manager = {
    .name = "buddy_pmm_manager",
    .init = buddy_init,
    .init_memmap = buddy_init_memmap,
    .alloc_pages = buddy_alloc_pages,
    .free_pages = buddy_free_pages,
    .nr_free_pages = buddy_nr_free_pages,
    .check = buddy_check,
};

