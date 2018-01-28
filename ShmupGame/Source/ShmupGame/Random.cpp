#include "Random.h"

#include <ctime>

Random::Random() {
    time_t timer = 0;
    init_genrand(static_cast<uint>(timer));
}

void Random::setSeed(long n) {
    init_genrand(static_cast<uint>(n));
}

int Random::nextInt(int n) {
    return genrand_int32() % n;
}

int Random::nextSignedInt(int n) {
    return genrand_int32() % (n * 2) - n;
}

float Random::nextFloat(float n) {
    return genrand_real1() * n;
}

float Random::nextSignedFloat(float n) {
    return genrand_real1() * (n * 2) - n;
}