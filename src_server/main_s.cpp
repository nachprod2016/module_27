#include "Server.h"
#include <thread>

int main()
{
    Server sr;
    std::thread t([&]()
    {
        sr.administrator();
    });
    sr.listenPort();
    t.join();
    return 0;
}