#include "clock.h"

#if defined(_WIN32)
#include <windows.h>
#endif

#include <chrono>

uint64_t Clock::millis()
{
#if defined(_WIN32)
    // For native builds on Windows.
    static const uint64_t start = GetTickCount64();
    const uint64_t now = GetTickCount64();
    return now - start;
#else
    using namespace std::chrono;
    static const auto start = steady_clock::now();
    const auto now = steady_clock::now();
    return (uint64_t)duration_cast<milliseconds>(now - start).count();
#endif
}

