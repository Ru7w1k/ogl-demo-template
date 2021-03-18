#include <Windows.h>
#include "helper.h"

LARGE_INTEGER prevTime;
LARGE_INTEGER curTime;
LARGE_INTEGER temp;

double freq = 0.0f;

// Initialize the clock
void InitClock()
{
    // get the frequency from OS
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&temp));
    freq = (static_cast<double>(temp.QuadPart)) / 1000.0;

    QueryPerformanceCounter(&prevTime);
    curTime = prevTime;
}

// Get current time in microseconds
float GetCurrentTimeMS()
{
    QueryPerformanceCounter(&temp);
    temp.QuadPart *= 10000;
    temp.QuadPart /= (LONGLONG)freq;

    return (float)temp.QuadPart;
}

// Get time different between now and previous call
float GetTimeDeltaMS()
{
    QueryPerformanceCounter(&temp);
    prevTime = curTime;
    curTime = temp;

    temp.QuadPart = curTime.QuadPart - prevTime.QuadPart;
    temp.QuadPart *= 10000;
    temp.QuadPart /= (LONGLONG)freq;

    return (float)temp.QuadPart;
}
