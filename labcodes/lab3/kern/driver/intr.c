#include <x86.h>
#include <intr.h>

/* intr_enable - enable irq interrupt */
void
intr_enable(void) {
    // asm volatile ("sti");
    sti();
}

/* intr_disable - disable irq interrupt */
void
intr_disable(void) {
    // asm volatile ("cli" ::: "memory");
    cli();
}

