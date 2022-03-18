#include <swap.h>
#include <swapfs.h>
#include <mmu.h>
#include <fs.h>
#include <ide.h>
#include <pmm.h>
#include <assert.h>

// swap file system init
void
swapfs_init(void) {

    // 检测一下PGSIZE、SECTSIZE数据有没有问题
    assert((PGSIZE % SECTSIZE) == 0);
    // static_assert((PGSIZE % SECTSIZE) == 0);

    // SWAP_DEV_NO = 1
    // ide设置检测
    if (!ide_device_valid(SWAP_DEV_NO)) {
        panic("swap fs isn't available.\n");
    }

    // (PGSIZE / SECTSIZE) = 8, 即一个page需要8个扇区
    // unsigned int size;          // Size in Sectors
    max_swap_offset = ide_device_size(SWAP_DEV_NO) / (PGSIZE / SECTSIZE);
}

int
swapfs_read(swap_entry_t entry, struct Page *page) {
    return ide_read_secs(SWAP_DEV_NO, swap_offset(entry) * PAGE_NSECT, page2kva(page), PAGE_NSECT);
}

int
swapfs_write(swap_entry_t entry, struct Page *page) {
    return ide_write_secs(SWAP_DEV_NO, swap_offset(entry) * PAGE_NSECT, page2kva(page), PAGE_NSECT);
}

