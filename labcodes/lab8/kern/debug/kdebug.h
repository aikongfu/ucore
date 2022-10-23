#ifndef __KERN_DEBUG_KDEBUG_H__
#define __KERN_DEBUG_KDEBUG_H__

#include <defs.h>
#include <trap.h>


#define SPLIT cprintf("------------------------\n")

#define DEBUG_LEVEL 2
#define __output(...) \
    cprintf(__VA_ARGS__); 

#if DEBUG_LEVEL == 1
#define __format(__fmt__) "<%s>: " __fmt__ "\n"
#define DEBUG(__fmt__, ...) \
         __output(__format(__fmt__), __FUNCTION__, ##__VA_ARGS__);
#elif DEBUG_LEVEL == 2
#define __format(__fmt__) "(%d)-<%s>: " __fmt__ "\n"
#define DEBUG(__fmt__, ...) \
         __output(__format(__fmt__), __LINE__, __FUNCTION__, ##__VA_ARGS__);
#else 
#define __format(__fmt__) "%s(%d)-<%s>: " __fmt__ "\n"
#define DEBUG(__fmt__, ...) \
         __output(__format(__fmt__), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);
#endif


void print_kerninfo(void);
void print_stackframe(void);
void print_debuginfo(uintptr_t eip);

#endif /* !__KERN_DEBUG_KDEBUG_H__ */

