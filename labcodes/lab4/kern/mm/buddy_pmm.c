#include <pmm.h>
#include <defs.h>
#include <list.h>
#include <string.h>
#include <buddy_pmm.h>
#include <stdio.h>
#include <stdio.h>


#define LEFT_LEAF(index) ((index) * 2 + 1)
#define RIGHT_LEAF(index) ((index) * 2 + 2)
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
    uint32_t longest[1024 * 1024];
};

// 记录分配块的信息
struct alloc_record {
    struct Page *base;
    uint32_t offset;
    size_t nr;      // 块大小
};

// 存入buddy信息，用来分配内存
struct buddy buddy_alloctor;
struct buddy *root = &buddy_alloctor;

// 存放偏移量的数组,记录全部已分配的信息
struct alloc_record alloced[1024 * 1024];

// 已分配的块数，从0开始
uint32_t nr_block = 0;

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
    cprintf("buddy_new, size = [%d], IS_POWER_OF_2(size) = [%d]\n", size, IS_POWER_OF_2(size));
    if (size < 1 || !IS_POWER_OF_2(size)) {
        return;
    }

    node_size = 2 * size;
    root->size = size;

    for (i = 0; i < 2 * size - 1; i++) {
        if (IS_POWER_OF_2(i + 1)) {
            node_size /= 2;
            cprintf("root->longest[%d] = [%d]\n", i, node_size);
        }

        root->longest[i] = node_size;
    }
    cprintf("\n");
    cprintf("buddy_new, root = [%d], root->longest= [%d], longest size = [%d]\n", size, IS_POWER_OF_2(size),
            sizeof(root->longest));

}

static uint32_t
buddy_alloc(struct buddy *self, uint32_t size) {
    uint32_t index = 0;
    uint32_t node_size;
    uint32_t offset;
    uint32_t left;
    uint32_t right;

    cprintf("buddy_alloc, self = [%p], root = [%p], self->size = [%d]\n", self->size, self, self->size);
    if (self == NULL) {
        cprintf("buddy_alloc, return -1\n");
        return -1;
    }

    if (size <= 0) {
        size = 1;
    } else if (!IS_POWER_OF_2(size)) {
        // 向上取2的n次幂
        size = uint32_round_up(size);
    }

    cprintf("buddy_alloc, self->longest[index] = [%d], size = [%d]\n", self->longest[index], size);
    // 无足够空间
    if (self->longest[index] < size) {
        return -1;
    }
    
    // 开始查找符合条件的节点
    for (node_size = self->size; node_size != size; node_size /= 2) {
        left = LEFT_LEAF(index);
        right = RIGHT_LEAF(index);

        // 如果两个子节点的空间都大于>=size，取较小的那个，都等于，则取左节点
        if (self->longest[left] >= size && self->longest[right] >= size) {
            index = self->longest[right] < self->longest[left] ? right : left;
            continue;
        }

        // 如果一个小于size,则取另一个
        if (self->longest[left] < size) {
            index = right;
        } else if (self->longest[right] < size) {
            index = left;
        }
    }

    self->longest[index] = 0;
    // 计算offset
    offset = (index + 1) * node_size - self->size;
    // 回滚，更改父节点的数据
    while (index) {
        index = PARENT(index);
        self->longest[index] = 
        MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
    }

    return offset;
}

static void buddy_free(struct buddy* self, uint32_t offset) {
    uint32_t node_size;
    uint32_t index = 0;
    uint32_t left_child_size, right_child_size;

    assert(self && offset >= 0 && self->size >= offset);
    
    node_size = 1;

    // index
    index = offset + self->size -1;

    // 向上寻找分配过的节点
    for (; self->longest[index]; index = PARENT(index)) {
        node_size *= 2;
        // 如果节点不存在
        if (index == 0) {
            return;
        }
    }

    self->longest[index] = node_size;

    while (index) {
        index = PARENT(index);
        node_size *= 2;
        left_child_size = self->longest[LEFT_LEAF(index)];
        right_child_size = self->longest[RIGHT_LEAF(index)];

        if (node_size == left_child_size + right_child_size) {
            self->longest[index] = node_size;
        } else {
            self->longest[index] = MAX(left_child_size, right_child_size);
        }
    }
    
}


static uint32_t buddy_size(struct buddy* self, uint32_t offset) {
    uint32_t node_size, index = 0;
    assert(self && offset >= 0 && offset <= self->size);

    node_size = 1;
    for (index = offset + self->size - 1; self->longest[index]; index = PARENT(index)) {
        node_size *= 2;
    }

    return node_size;
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
        list_add(&free_list, &(p->page_link));
    }

    // 这里只是简化处理，这样存在一个问题，真正可用的页数就是小于等于n的最大的一个2^k次幂，所以size - n 将浪费掉，也可以继续分割（size - n）
    uint32_t size = uint32_round_down(n);
    base->property = n;
    nr_free += n;
    SetPageProperty(base);
    buddy_new(size);
    cprintf("buddy_init_memmap, n = [%d], size = [%d], root size = [%d]\n", n, size, sizeof(*root));
    cprintf("buddy_init_memmap, root = [%p], root->size = [%d], root->longest = [%p]\n", root, root->size, root->longest);
}


// 分配的逻辑是：
// 1、首先在buddy的“二叉树”结构中找到应该分配的物理页在整个实际双向链表中的位置
// 2、而后把相应的page进行标识表明该物理页已经分出去了。
static struct Page *
buddy_alloc_pages(size_t n) {
    int i;
    int alloc_size = n;
    struct Page* page = NULL;
    struct Page* p;
    assert(n > 0);
    if (n > nr_free) {
        return NULL;
    }

    if (!IS_POWER_OF_2(n)) {
        alloc_size = uint32_round_up(n);
    }

    // 记录偏移量
    uint32_t offset = buddy_alloc(root, alloc_size);
    alloced[nr_block].offset = offset;
    list_entry_t *le = list_next(&free_list);

    for (i = 0; i < offset; i++) {
        le = list_next(le);
    }

    page = le2page(le, page_link);
    
    // 根据需求n得到块大小
    // 记录分配块首页
    // 记录分配的页数
    alloced[nr_block].base = page;
    alloced[nr_block].nr = alloc_size;
    for (p = page; p != page + alloc_size; p++) {
        ClearPageProperty(p);
    }

    page->property = alloc_size;
    cprintf("buddy_alloc_pages, need page = [%d], nr_block = [%d], page = [%p], nr = [%d], offset = [%d]\n", 
            n, nr_block, page, alloc_size, offset);

    nr_free -= alloc_size;
    nr_block++;
    return page;
}


/**
 * @brief 遍历alloced数组，找到对应页page，及相关信息
 *        回收需要回收的，回溯更新buddy节点信息，需要注意的可能当时分配的是100页（真正分配128页），现在回收的是40页
 *        此时，因为40并非是2的整次幂，其往下2的整次幂为32，往上2的整次幂为64， 
 *        那么如果回收64，会把还在用的页回收掉，因而需要32，8这样分多次去回收，直到全部回收，需要更新alloced
 * 
 * @param base 
 * @param n 
 */
static void
buddy_free_pages(struct Page *base, size_t n) {
    uint32_t node_size, index = 0;
    uint32_t left_longest, right_longest;
    uint32_t position = 0;
    struct buddy* self = root;
    int i = 0;
    for (i = 0; i < nr_block; i++) {
        if (alloced[i].base == base) {
            break;
        }
    }


    uint32_t offset = alloced[i].offset;
    position = i;
    i = 0;
    list_entry_t *le = list_next(&free_list);

    while (i < offset) {
        le = list_next(le);
        i++;
    }

    int allocPages = n;
    if (!IS_POWER_OF_2(n)) {
        allocPages = uint32_round_up(n);
    }

    assert(self && offset >= 0 && offset <= self->size);

    nr_free += allocPages;
    struct Page *p;
    for (i = 0; i < allocPages; i++) {
        p = le2page(le, page_link);
        p->flags = p->property = 0;
        set_page_ref(p, 0);
        le = list_next(le);
    }

    buddy_free(root, offset);

    nr_block--;
}

static size_t
buddy_nr_free_pages(void) {
    return nr_free;
}

static void 
buddy_check(void) {
    struct Page *p0, *A, *B, *C, *D;
    p0 = alloc_page();
    A = alloc_page();
    B = alloc_page();

    cprintf("p0 = [%p]\n", p0);
    cprintf("A = [%p]\n", A);
    cprintf("B = [%p]\n", B);
    assert(p0 != NULL);
    assert(A != NULL);
    assert(B != NULL);

    assert(p0 != A && p0 != B && A != B);
    free_page(p0);
    free_page(A);
    free_page(B);

    A = alloc_pages(500);
    B = alloc_pages(500);
    cprintf("A %p\n", A);
    cprintf("B %p\n", B);

    free_pages(A, 500);
    free_pages(B, 500);

    // p0=alloc_pages(1024);
    // cprintf("p0 %p\n",p0);
    // assert(p0 == A);
    // //以下是根据链接中的样例测试编写的
    // A=alloc_pages(70);  
    // B=alloc_pages(35);
    // assert(A+128==B);//检查是否相邻
    // cprintf("A %p\n",A);
    // cprintf("B %p\n",B);
    // C=alloc_pages(80);
    // assert(A+256==C);//检查C有没有和A重叠
    // cprintf("C %p\n",C);
    // free_pages(A,70);//释放A
    // cprintf("B %p\n",B);
    // D=alloc_pages(60);
    // cprintf("D %p\n",D);
    // assert(B+64==D);//检查B，D是否相邻
    // free_pages(B,35);
    // cprintf("D %p\n",D);
    // free_pages(D,60);
    // cprintf("C %p\n",C);
    // free_pages(C,80);
    // free_pages(p0,1000);//全部释放

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

