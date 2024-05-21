#include "SKSEUtil/Random.h"
#include <ctime>

namespace SKSEUtil {

	Random::Random()
	{
	}

	void Random::seedByCurrentSecond()
	{
		mEngine.seed(static_cast<unsigned int>(time(NULL)));
	}

	unsigned int Random::nextValue()
	{
		unsigned int val = mEngine();
		return val;
	}

	int Random::randomInt(int min, int max_exclusive) {
		int range = max_exclusive - min;
		return (nextValue() % range) + min;
	}

	unsigned int Random::randomUnsignedInt(unsigned int min, unsigned int max_exclusive) {
		unsigned int range = max_exclusive - min;
		return (nextValue() % range) + min;
	}

	float Random::randomFloat(float min, float max)
	{
		return ((float)nextValue() / (float)UINT_MAX) * (max - min) + min;
	}

}