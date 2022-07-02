#include "kernel.h"
#include "posix_board_if.h"
#include <zephyr/zephyr.h>
#include "active_object.hpp"

#define MY_STACK_SIZE 500
#define MY_PRIORITY 5


class MyActive : private ActiveObject
{
    public:
        MyActive(k_thread_stack_t *stack, size_t stack_size, int prio, k_timeout_t delay) : ActiveObject(stack, stack_size, prio, delay){};
        void run()
        {
            while(true){
                printk("Hello\n");
                k_sleep(K_MSEC(500));
            }        
        }
        ~MyActive() = default;
};
K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);


class FooActive : private ActiveObject
{
    public:
        FooActive(k_thread_stack_t *stack, size_t stack_size, int prio, k_timeout_t delay) : ActiveObject(stack, stack_size, prio, delay){};
        void run()
        {
            while(true){
                printk("Goodbye\n");
                k_sleep(K_MSEC(500));
            }        
        }
        ~FooActive() = default;
};
K_THREAD_STACK_DEFINE(FooActive_stack_area, MY_STACK_SIZE);


void main(void)
{
    printk("\nStartup...\n\n");
    MyActive active_object(my_stack_area, K_THREAD_STACK_SIZEOF(my_stack_area), MY_PRIORITY, K_NO_WAIT);
    FooActive foo_active_object(FooActive_stack_area, K_THREAD_STACK_SIZEOF(my_stack_area), MY_PRIORITY, K_NO_WAIT);
    k_sleep(K_FOREVER);
    posix_exit(0);
}
