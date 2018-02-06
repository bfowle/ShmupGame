#include "Random.h"

#include <ctime>

//using namespace std;

Random::Random() {
    //unsigned long init[4] = { 0x123, 0x234, 0x345, 0x456 }, length = 4;
    //init_by_array(init, length);

    time_t timer = time(NULL);
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