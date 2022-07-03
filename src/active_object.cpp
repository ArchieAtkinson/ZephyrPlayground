#include "active_object.hpp"
#include "kernel.h"
#include "sys_clock.h"
#include <algorithm>
#include <assert.h>
ActiveObject::ActiveObject(k_thread_stack_t *stack, size_t stack_size, int prio,
                           char *msg_buff, size_t msg_buff_size)
    : m_stack{stack}, m_stack_size{stack_size}, m_prio{prio} ,m_msg_buff {msg_buff},
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

void ActiveObject::start()
{
  init();
  m_thread_id = k_thread_create(&m_thread_data, m_stack, m_stack_size, entry_point, this, NULL, NULL, m_prio, 0, K_NO_WAIT);
}

void ActiveObject::receive()
{
    struct event_data_t data;
    while(true){
        k_msgq_get(get_msgq(), &data, K_FOREVER);
        run(data);
    }       
}

void ActiveObject::add_subscriber(ActiveObject *subscriber, my_events_t event) {
    assert(m_subscribers.find(event) != m_subscribers.end());
    m_subscribers.at(event).push_back(subscriber);
}


void ActiveObject::remove_subscriber(ActiveObject* sub, my_events_t event)
{
    std::vector<ActiveObject *> *  event_vec = &(m_subscribers.at(event));
    event_vec->erase(std::remove(event_vec->begin(), event_vec->end(), sub), event_vec->end());
}

void ActiveObject::subscribe(ActiveObject *producer, my_events_t event)
{
    producer->add_subscriber(this, event);
}


void ActiveObject::unsubscribe(ActiveObject *producer, my_events_t event)
{
    producer->remove_subscriber(this, event);
}


void  ActiveObject::producer_declare(my_events_t event)
{
  m_subscribers.insert({event,{}});
}

void ActiveObject::post_event(struct event_data_t *data, k_timeout_t timeout) {
  for(auto &&sub : m_subscribers.at(data->event)){
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