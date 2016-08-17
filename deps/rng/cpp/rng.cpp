#include "rng/headers/rng.hpp"

static uint32_t value;

void rngSeed(uint32_t seed)
{
	value = seed;
}

uint32_t rngGet()
{
	value = (1103515245 * value + 12345) & 0xFFFFFFFF;
	return value;
}
