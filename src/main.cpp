#include "kernel.h"
#include "posix_board_if.h"
#include <zephyr/zephyr.h>
#include "active_object.hpp"

#define MY_STACK_SIZE 500
#define MY_PRIORITY 5
#include <stdio.h>


class MyActive : public ActiveObject
{
    using ActiveObject::ActiveObject;
    public:
        void run(struct event_data_t data)
        {
            switch (data.event) {
                case my_events_t::event1:
                {
                    char * msg = static_cast<char*>(data.data);
                    printk("Hello and %s\n", msg);
                    break;
                }
                default:
                    printk("Not listed\n");
            }


        }
        ~MyActive() = default;
};
K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);
char __aligned(4) my_msgq_buffer[10 * sizeof(struct ActiveObject::event_data_t)];


char __aligned(4) foo_msgq_buffer[10 * sizeof(struct ActiveObject::event_data_t)];

void main(void)
{
    printk("\nStartup...\n\n");
    MyActive active_object(my_stack_area, K_THREAD_STACK_SIZEOF(my_stack_area), MY_PRIORITY, K_NO_WAIT, my_msgq_buffer, 10);

    int count = 0;
    struct ActiveObject::event_data_t data;
    char msg[10];
    while(true){
        sprintf(msg, "%d", count); 
        data.event = my_events_t::event1;
        data.data = static_cast<void*>(msg);
        k_msgq_put(active_object.get_msgq(), &data, K_FOREVER);
        count++;
        k_sleep(K_MSEC(500));
    }        
    posix_exit(0);
}
