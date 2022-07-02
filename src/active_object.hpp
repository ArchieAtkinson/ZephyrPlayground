#pragma once
#include <zephyr/zephyr.h>

class ActiveObject
{
    public:
        ActiveObject() = delete; // default constuctor
        virtual ~ActiveObject() = default;

        ActiveObject( const ActiveObject& ) = delete; // non construction-copyable
        ActiveObject& operator=( const ActiveObject& ) = delete; // non copyable
        
    protected:
        ActiveObject(k_thread_stack_t *stack, size_t stack_size, int prio, k_timeout_t delay);
        
    private:
        static void entry_point(void *self, void *, void *);
        virtual void run() = 0;

        const k_tid_t m_thread_id;
        struct k_thread m_thread_data;
};