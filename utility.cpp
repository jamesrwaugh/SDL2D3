#include "utility.h"

float meters(int pixels)
{
    return pixels * conf::mpp;
}

float pixels(long double meters)
{
    return meters * conf::ppm;
}

float operator"" px(unsigned long long int _pixels) {
    return meters(_pixels);
}
