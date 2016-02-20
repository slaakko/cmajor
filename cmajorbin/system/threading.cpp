#include <thread>

extern "C" int get_hardware_concurrency()
{
    return std::thread::hardware_concurrency();
}
