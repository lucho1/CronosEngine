#ifndef _PERFTIMER_H_
#define _PERFTIMER_H_

#include "Globals.h"

namespace Cronos {

	class PerfTimer
	{
	public:

		// Constructor
		PerfTimer();

		void Start();
		double ReadMs() const;
		uint64 ReadTicks() const;

	private:
		uint64	started_at;
		static uint64 frequency;
	};

}

#endif