#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>

std::mutex mtx;
void add_a_mil(int& val)
{
    mtx.lock();
    for (size_t i{}; i < 1'000'000; ++i)
    {
        ++val;
    }
    mtx.unlock();
}

int main(int argc, char** argv)
{
    int c = 0;
    std::thread t1(add_a_mil, std::ref(c));
    std::thread t2(add_a_mil, std::ref(c));

    t1.join();
    t2.join();
    std::cout << c;
    return 0;
}