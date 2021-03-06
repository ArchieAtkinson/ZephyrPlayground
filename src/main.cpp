#include "kernel.h"
#include "posix_board_if.h"
#include <zephyr/zephyr.h>

void thread1(void *a, void *b, void *c) {
  
    k_sleep(K_SECONDS(10));
    while (true)
    {
        printk("Thread1\n");
        k_busy_wait(100000);
    }
}

K_THREAD_DEFINE(thread1_id, 500, thread1, NULL, NULL, NULL, 5, 0, 0);

void thread2(void *a, void *b, void *c)
{
    while (true)
    {
        printk("Thread2\n");
        k_busy_wait(100000);
    }
}
K_THREAD_DEFINE(thread2_id, 500, thread2, NULL, NULL, NULL, 6, 0, 0);

void main(void)
{
    printk("Hello! %s\n", CONFIG_BOARD);

    k_sleep(K_FOREVER);

    posix_exit(0);
}
