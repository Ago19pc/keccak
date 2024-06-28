#pragma once

#include <stdio.h>

#ifdef logger
    #define LOG(fmt, ...) printf(fmt, __VA_ARGS__)
#else
    #define LOG(a, b)
#endif
