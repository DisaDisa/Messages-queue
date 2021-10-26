#pragma once
#include <queue>
#include <semaphore.h>
#include <mutex>
#include <condition_variable>
#include <memory>

template <typename T>
struct messages_queue {

    explicit messages_queue(const std::size_t capacity);

    //thread safe. No wait
    bool try_push(const T& new_element);

    T wait_pop(); //thread safe. Wait

    //thread safe. No wait
    bool try_pop(T& val);

    std::size_t size() const;

private:

    std::queue <T> queue;
    std::size_t capacity;
    mutable std::mutex mtx;
    std::condition_variable cond;
};


#include "messages_queue.cpp"