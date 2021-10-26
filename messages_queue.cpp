#include <iostream>
#include <stdexcept>
#include <future>

template <typename T>
messages_queue<T>::messages_queue(std::size_t cap) : capacity(cap) {}

template <typename T>
bool messages_queue<T>::try_push(const T& new_element) {
    std::lock_guard lk(mtx);
    std::cout << "Queue push. Old size = " << queue.size() << std::endl; //It's only for demonstration
    if (queue.size() == capacity) {
        std::cout << "Queue is full. Return" << std::endl; //It's only for demonstration
        return false;
    }
    queue.push(new_element);
    cond.notify_one();
    return true;
}


template <typename T>
T messages_queue<T>::wait_pop() {
    std::unique_lock lk(mtx);
    std::cout << "Queue pop. Old size = " << queue.size() << std::endl; //It's only for demonstration
    if (queue.size() == 0) {
        std::cout << "Queue is empty. Wait" << std::endl; //It's only for demonstration
    }
    cond.wait(lk, [this]() {return !queue.empty();});
    auto ans = queue.front();
    queue.pop();
    std::cout << "Queue pop end" << std::endl; //It's only for demonstration
    return ans;
}

template <typename T>
bool messages_queue<T>::try_pop(T& val) {
    std::lock_guard lk(mtx);
    if (queue.empty()) {
        return false;
    }
    val = queue.front();
    return true;
}

template <typename T>
std::size_t messages_queue<T>::size() const {
    std::lock_guard lk(mtx);
    return queue.size();
}
