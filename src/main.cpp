#include "events.hpp"
#include "kernel.h"
#include "posix_board_if.h"
#include <zephyr/zephyr.h>
#include "active_object.hpp"
#include "sys_clock.h"

#define MY_STACK_SIZE 500
#define MY_PRIORITY 5
#include <stdio.h>


struct ActiveObject::event_data_t my_const_data = {.event = my_events_t::event1, .data = nullptr};
struct ActiveObject::event_data_t my_const_data_2 = {.event = my_events_t::event2,
                                                   .data = nullptr};

class AO_1 : public ActiveObject
{
  using ActiveObject::ActiveObject;

    public:
        ~AO_1() = default;
    protected:
        void run(struct event_data_t data) override
        {
            switch (data.event) {
                case my_events_t::event1:
                {
                    printk("Hello_1_EVENT1\n");
                    post_event(&my_const_data, K_FOREVER);
                    k_sleep(K_MSEC(500));
                    break;
                }
                case my_events_t::event2:
                {
                    printk("Hello_1_EVENT2\n");
                    k_sleep(K_MSEC(500));
                    break;
                }
                default:
                    printk("Not listed\n");
            }


        }
        void init() override
        {
            producer_declare(my_events_t::event1);
            producer_declare(my_events_t::event2);
            producer_declare(my_events_t::no_event);
        }


};
K_THREAD_STACK_DEFINE(ao_1_stack, MY_STACK_SIZE);
char __aligned(4) ao_1_msgq_buf[10 * sizeof(struct ActiveObject::event_data_t)];

class AO_2 : public ActiveObject
{
    using ActiveObject::ActiveObject;
    public:
        ~AO_2() = default;
    public:
        void run(struct event_data_t data) override
        {
            switch (data.event) {
                case my_events_t::event1:
                {
                    printk("Hello_2\n");
                    post_event(&my_const_data, K_FOREVER);
                    post_event(&my_const_data_2, K_FOREVER);
                    k_sleep(K_MSEC(500));
                    break;
                }
                default:
                    printk("Not listed\n");
            }
        }
        void init() override
        {
            producer_declare(my_events_t::event1);
            producer_declare(my_events_t::event2);
            producer_declare(my_events_t::no_event);
        }
};
K_THREAD_STACK_DEFINE(ao_2_stack, MY_STACK_SIZE);
char __aligned(4) ao_2_msgq_buf[10 * sizeof(struct ActiveObject::event_data_t)];




void main(void)
{
    printk("\nStartup...\n\n");
    AO_1 ao_1(ao_1_stack, K_THREAD_STACK_SIZEOF(ao_1_stack), MY_PRIORITY, ao_1_msgq_buf, 10);
    AO_2 ao_2(ao_2_stack, K_THREAD_STACK_SIZEOF(ao_2_stack), MY_PRIORITY,
              ao_2_msgq_buf, 10);

    ao_1.start();
    ao_2.start();

    ao_1.subscribe(&ao_2, my_events_t::event1);
    ao_1.subscribe(&ao_2, my_events_t::event2);

    ao_2.subscribe(&ao_1, my_events_t::event1);
    
    ao_1.post_event(&my_const_data,K_FOREVER);

    k_sleep(K_FOREVER);
    posix_exit(0);
}
