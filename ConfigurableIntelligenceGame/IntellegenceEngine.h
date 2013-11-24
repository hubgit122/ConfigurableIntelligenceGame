
#ifndef __INTELLEGENCEENGINE_H__
#define __INTELLEGENCEENGINE_H__

#include "Player.h"

namespace CIG
{
	class IntellegenceEngine: public Player
	{
		public:
			IntellegenceEngine(int POWER_ );
			virtual ~IntellegenceEngine();
			void operator = (const IntellegenceEngine& ie);
			int POWER;											//一种能力设定.
			static const float MAX_SEARCH_TIME;
	};

	const float IntellegenceEngine::MAX_SEARCH_TIME = 10000;		//10s

}

#endif /*__INTELLEGENCEENGINE_H_*/

