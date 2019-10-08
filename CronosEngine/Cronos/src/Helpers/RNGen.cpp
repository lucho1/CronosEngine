#include "Providers/cnpch.h"
#include "RNGen.h"

namespace Cronos {

	RNGen::RNGen()
	{
		m_RNEngine = std::default_random_engine(m_RandomDevice());
	}

	RNGen::~RNGen()
	{
	}


	//----------------- RANDOM INT GENERATION -----------------
	int RNGen::GetIntRN()
	{
		return m_IntDistribution(m_RNEngine);
	}

	int RNGen::GetIntRNInRange(int rangeMin_Value, int rangeMaxValue)
	{
		std::uniform_int_distribution<int> tmp_IntDistribution(rangeMin_Value, rangeMaxValue);
		return tmp_IntDistribution(m_RNEngine);
	}

	std::vector<int> RNGen::GetIntRNVecInRange(int vecSize, int rangeMin_Value, int rangeMaxValue)
	{
		std::uniform_int_distribution<int> tmp_IntDistribution(rangeMin_Value, rangeMaxValue);

		std::vector<int> ret;
		for (int i = 0; i < vecSize; i++)
			ret.push_back(tmp_IntDistribution(m_RNEngine));

		return ret;
	}

	std::vector<int> RNGen::GetIntRNVec(int vecSize)
	{
		std::uniform_int_distribution<int> tmp_IntDistribution;

		std::vector<int> ret;
		for (int i = 0; i < vecSize; i++)
			ret.push_back(tmp_IntDistribution(m_RNEngine));

		return ret;
	}


	//----------------- RANDOM DOUBLE GENERATION -----------------
	double RNGen::GetDoubleRN()
	{
		return m_DoubleDistribution(m_RNEngine);
	}

	std::vector<double> RNGen::GetDoubleRNVecInRange(int vecSize, double rangeMin_Value, double rangeMaxValue)
	{
		std::uniform_real_distribution<double> tmp_DoubleDistribution(rangeMin_Value, rangeMaxValue);

		std::vector<double> ret;
		for (int i = 0; i < vecSize; i++)
			ret.push_back(tmp_DoubleDistribution(m_RNEngine));

		return ret;
	}

	std::vector<double> RNGen::GetDoubleRNVec(int vecSize)
	{
		std::uniform_real_distribution<double> tmp_DoubleDistribution;

		std::vector<double> ret;
		for (int i = 0; i < vecSize; i++)
			ret.push_back(tmp_DoubleDistribution(m_RNEngine));

		return ret;
	}

}