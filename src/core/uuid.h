#ifndef _UUID_H_
#define _UUID_H_

#include <cstdint>
#include <random>



namespace
{
	std::random_device rd;
	std::mt19937 rngEngine(rd());
	std::uniform_int_distribution<uint32_t> uniformDistr;
}



namespace ruya
{
	/*
	* Universal Unique ID.
	* Will be used to distinguish between entities such as Objects, ...
	*/
	struct UUID
	{
		using uuid_type = std::uint32_t;
		uuid_type value {};

		friend bool operator==(UUID, UUID) = default;
	};



	inline UUID generate_uuid()
	{
		return UUID {uniformDistr(rngEngine)};
	}
}

#endif // _UUID_H_

