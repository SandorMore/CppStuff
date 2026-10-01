#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>

namespace Saso {
    class jthread final 
    {
    public:
        jthread() noexcept = default;
        
        jthread(const jthread&) = delete;
    
        jthread operator=(const jthread&) = delete;

        template <typename Callable, typename Args&&...>
        explicit jthread(Callable&& func, Args&&... args)
            : t{std::forward<Callable>(func), std::forward<Args>(args)...}
        {
            
        }

        explicit jthread(std::thread t_)
        {
            t(std::move(t_));
        }

        jthread(jthread&& other) noexcept
        {
            this->t(std::move(other));
        }

        jthread& operator=(jthread&& other)
        {
            if(joinable())
                join();
            
            t = std::move(other);
            return *this;   
        }

        ~jthread()
        {
            if(joinable())
                join();
        }
    private:
        std::thread t;
    };
}


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

//xddd
long create_unique_thread_id(std::thread* thread)
{
    return *(reinterpret_cast<long*>(thread));
}

int main(int argc, char** argv)
{
    int c = 0;

    std::thread t1(add_a_mil, std::ref(c));
    std::thread t2(add_a_mil, std::ref(c));

    std::cout << "Thread 1 id is" << create_unique_thread_id(&t1) << "\n";
    std::cout << "Thread 2 id is" << create_unique_thread_id(&t2) << "\n";
    
    t1.join();
    t2.join();

    std::cout << c;
    
    return 0;
}