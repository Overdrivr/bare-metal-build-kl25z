#ifndef RNG_HPP_
#define RNG_HPP_

#include "stdint.h"

void rngSeed(uint32_t seed);
uint32_t rngGet();

#endif
