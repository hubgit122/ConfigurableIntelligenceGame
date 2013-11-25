
#ifndef __INTELLEGENCEENGINE_H__
#define __INTELLEGENCEENGINE_H__

#include "Player.h"

namespace CIG
{
	//´¿½Ó¿Ú. 
	class IntellegenceEngine: public Player
	{
		public:
			IntellegenceEngine(CIGRuleConfig::PLAYER_NAMES p = CIGRuleConfig::PLAYER_NAMES(-1) , Chessboard* cb = NULL);
			IntellegenceEngine(const IntellegenceEngine& ie);
			virtual ~IntellegenceEngine();
	};
}

#endif /*__INTELLEGENCEENGINE_H_*/

