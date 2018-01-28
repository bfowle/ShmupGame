#ifndef RANDOM_H
#define RANDOM_H

#include "mt.h"

#include <ctime>
#include <cstdlib>

class Random {
public:
    Random();
    ~Random() {};

    void setSeed(long n);
    int nextInt(int n);
    int nextSignedInt(int n);
    float nextFloat(float n);
    float nextSignedFloat(float n);
};

#endif