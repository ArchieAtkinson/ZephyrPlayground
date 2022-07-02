#pragma once
#include <zephyr/zephyr.h>
#include "events.hpp"

class ActiveObject
{
    public:
        ActiveObject() = delete; // default constuctor
        virtual ~ActiveObject() = default;

        ActiveObject( const ActiveObject& ) = delete; // non construction-copyable
        ActiveObject& operator=( const ActiveObject& ) = delete; // non copyable

        ActiveObject(k_thread_stack_t *stack, size_t stack_size, int prio, k_timeout_t delay, char* msg_buff, size_t msg_buff_size);

        struct event_data_t {
            my_events_t event;
            void* data;
        }__attribute__((aligned(4)));

        struct k_msgq* get_msgq();

    protected:
        k_tid_t get_thread_id() const;
        struct k_thread get_thread_data() const;
        
    private:
        static void entry_point(void *self, void *, void *);
        void receive();
        virtual void run(event_data_t data) = 0;
        struct k_thread m_thread_data;
        const k_tid_t m_thread_id;
        
        struct k_msgq m_msgq;
        char* m_msg_buff;
        size_t m_msg_buff_size;
};