#include "uuid.h"
#include <random>


namespace
{
	std::random_device rd;
	std::mt19937_64 rngEngine(rd());
	std::uniform_int_distribution<uint64_t> uniformDistr;
}

ruya::UUID::UUID()
	: mUUID{uniformDistr(rngEngine)}
{

}


bool ruya::UUID::operator==(const UUID& other) const
{
	return this->mUUID == other.mUUID;
}
