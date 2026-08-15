#ifndef _UUID_H_
#define _UUID_H_

#include <cstdint>
#include <random>



namespace
{
	std::mt19937 rng_engine(std::random_device{}());
}



namespace ruya
{
	inline uint32_t rand_int(uint32_t min, uint32_t max)
	{
		std::uniform_int_distribution<uint32_t> uniform_int{min, max};
		return uniform_int(rng_engine);
	}
}

#endif // _UUID_H_

