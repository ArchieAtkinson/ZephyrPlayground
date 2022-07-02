#include "active_object.hpp"
#include "sys_clock.h"
#include <algorithm>

ActiveObject::ActiveObject(k_thread_stack_t *stack, size_t stack_size, int prio, k_timeout_t delay, char* msg_buff, size_t msg_buff_size) 
    :   m_thread_id {k_thread_create(&m_thread_data, stack, stack_size, entry_point, this, NULL, NULL, prio, 0, delay)},
        m_msg_buff {msg_buff},
        m_msg_buff_size {msg_buff_size}
{
  k_msgq_init(&m_msgq, m_msg_buff, sizeof(struct event_data_t),
              m_msg_buff_size);
};

void ActiveObject::entry_point(void *self, void *, void *)
{
    auto active_object = static_cast<ActiveObject *>(self);
    active_object->receive();
};

void ActiveObject::receive()
{
    struct event_data_t data;
    while(true){
        k_msgq_get(get_msgq(), &data, K_FOREVER);
        run(data);
    }       
}

void ActiveObject::subscribe(ActiveObject *sub) { subscribers.push_back(sub); }


void ActiveObject::unsubscribe(ActiveObject* sub)
{
    subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), sub), subscribers.end());
}

void ActiveObject::post_event(struct event_data_t *data, k_timeout_t timeout) {
  for(auto &&sub : subscribers){
    k_msgq_put(sub->get_msgq(), data, timeout);
  }
}

struct k_msgq* ActiveObject::get_msgq()
{
    return &m_msgq;
}

k_tid_t ActiveObject::get_thread_id() const
{
    return m_thread_id;
}

struct k_thread ActiveObject::get_thread_data() const
{
    return m_thread_data;
}