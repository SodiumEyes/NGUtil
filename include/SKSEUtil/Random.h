#pragma once

#include <random>

namespace SKSEUtil {

	class Random
	{
	private:
		std::mt19937 mEngine;

	public:
		Random();

		void seedByCurrentSecond();

		unsigned int nextValue();

		int randomInt(int min, int max_exclusive);
		unsigned int randomUnsignedInt(unsigned int min, unsigned int max_exclusive);
		float randomFloat(float min, float max);
	};

}