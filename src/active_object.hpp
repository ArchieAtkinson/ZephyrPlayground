#pragma once
#include <zephyr/zephyr.h>
#include "events.hpp"
#include <vector>
#include <map>

class ActiveObject
{
    public:
        ActiveObject() = delete; // default constuctor
        virtual ~ActiveObject() = default;

        ActiveObject( const ActiveObject& ) = delete; // non construction-copyable
        ActiveObject& operator=( const ActiveObject& ) = delete; // non copyable

        ActiveObject(k_thread_stack_t *stack, size_t stack_size, int prio, char* msg_buff, size_t msg_buff_size);

        void add_subscriber(ActiveObject* sub, my_events_t event);
        void remove_subscriber(ActiveObject *sub, my_events_t event);

        void subscribe(ActiveObject* producer, my_events_t event);
        void unsubscribe(ActiveObject* producer, my_events_t event);

        struct event_data_t {
            my_events_t event;
            void* data;
        }__attribute__((aligned(4)));

        struct k_msgq *get_msgq();
        void post_event(struct event_data_t *data, k_timeout_t timeout);
        void start();

    protected:
        k_tid_t get_thread_id() const;
        struct k_thread get_thread_data() const;
        void producer_declare(my_events_t event);
        
    private:
        static void entry_point(void *self, void *, void *);
        void receive();
        virtual void run(event_data_t data) = 0;
        virtual void init() = 0;
        
        struct k_thread m_thread_data;
        k_tid_t m_thread_id;
        k_thread_stack_t *m_stack;
        size_t m_stack_size;
        int m_prio;
        
        struct k_msgq m_msgq;
        char* m_msg_buff;
        size_t m_msg_buff_size;
        

        std::map<my_events_t, std::vector<ActiveObject *>> m_subscribers;
};