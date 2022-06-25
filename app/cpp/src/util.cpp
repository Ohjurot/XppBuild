#include "util.h"

#ifdef MY_OFF
#define C_OFF MY_OFF
#else
#define C_OFF 0
#endif

float my_util::my_sin(float off, float v)
{
    return off + std::sinf(v) + C_OFF;
}
