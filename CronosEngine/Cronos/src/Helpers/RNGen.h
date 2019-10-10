#ifndef _RNGEN_H_
#define _RNGEN_H_

#include <random>

namespace Cronos {

	class RNGen
	{

	public:

		RNGen();
		~RNGen();


		int GetIntRN(); //Shouldn't give negative numbers
		int GetIntRNInRange(int rangeMin_Value, int rangeMaxValue);
		std::vector<int>GetIntRNVecInRange(int vecSize, int rangeMin_Value, int rangeMaxValue);
		std::vector<int>GetIntRNVec(int vecSize);

		double GetDoubleRN();
		std::vector<double>GetDoubleRNVecInRange(int vecSize, double rangeMin_Value, double rangeMaxValue);
		std::vector<double>GetDoubleRNVec(int vecSize);


	private:

		std::default_random_engine m_RNEngine;
		std::random_device m_RandomDevice;

		std::uniform_real_distribution<double> m_DoubleDistribution;
		std::uniform_int_distribution<int> m_IntDistribution;

	};
}

#endif