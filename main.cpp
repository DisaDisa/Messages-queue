#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>
#include "messages_queue.hpp"

using namespace std;

void test_no_waits() {
    messages_queue<int> q(2);
    for (int i = 0; i < 2; i++) {
        q.try_push(i);
    }
    cout << q.wait_pop() << '\n';
    cout << q.wait_pop() << '\n';
}

void test_wait() {
    messages_queue<int> q(2);
    for (int i = 0; i < 2; i++) {
        q.try_push(10 + i);
    }
    cout << q.wait_pop() << '\n';
    cout << q.wait_pop() << '\n';
    cout << q.wait_pop() << '\n';
}

void test_full_queue() {
    messages_queue<int> q(2);
    for (int i = 0; i < 4; i++) {
        q.try_push(20 + i);
    }
}

std::mutex mtx_console;

const unsigned int writer_sleep_from = 4;
const unsigned int writer_sleep_to = 6;
const unsigned int reader_sleep = 5;

void writer(messages_queue <int> &q, const int th_num) {
    srand(time(0));
    unsigned int cnt = 0;
    while (1) {
        unsigned int sec = rand() % (writer_sleep_to - writer_sleep_from + 1) + writer_sleep_from;
        std::this_thread::sleep_for(std::chrono::seconds(sec));
        q.try_push(cnt++);
        std::lock_guard <std::mutex> lck(mtx_console);
        //std::cout << "Tread writer num = " << th_num << " push " << cnt << " Queue size = " << q.size() << endl; 
    }
}

void reader(messages_queue <int> &q, const int th_num) {
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(reader_sleep));
        q.wait_pop();
        std::lock_guard <std::mutex> lck(mtx_console);
        //std::cout << "Tread reader num = " << th_num << " pop " << res << " Queue size = " << q.size() << endl;
    }
}

void test_simple_emulator(unsigned int queue_size, unsigned int cnt_writers, unsigned int cnt_readers) {
    messages_queue<int> q(queue_size);
    std::thread th_writer[cnt_writers];
    std::thread th_reader[cnt_readers];
    for (unsigned int i = 0; i < cnt_writers; i++) {
        th_writer[i] = std::thread(writer, std::ref(q), i);
    }
    for (unsigned int i = 0; i < cnt_readers; i++) {
        th_reader[i] = std::thread(reader, std::ref(q), i);
    }

    for (unsigned int i = 0; i < cnt_writers; i++) {
        th_writer[i].join();
    }
    for (unsigned int i = 0; i < cnt_readers; i++) {
        th_reader[i].join();
    }
}


int main(int argc, char **argv) {
    //test_no_waits();
    //test_wait();
    //test_full_queue();
    if (argc != 4) {
        cout << "Usage:\nFirst arg - Queue size\nSecond arg - num of writers\nThird arg - num of readers\n";
        return 0;
    }
    test_simple_emulator(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    return 0;
}