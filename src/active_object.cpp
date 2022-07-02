#include "active_object.hpp"

ActiveObject::ActiveObject(k_thread_stack_t *stack, size_t stack_size, int prio, k_timeout_t delay) 
    : m_thread_id {k_thread_create(&m_thread_data, stack, stack_size, entry_point, this, NULL, NULL, prio, 0, delay)}
{
    
};

void ActiveObject::entry_point(void *self, void *, void *)
{
    auto active_object = static_cast<ActiveObject*>(self);
    active_object->run();
};