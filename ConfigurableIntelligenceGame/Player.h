
#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "utilities.h"
#include "CIGNamedObject.h"
#include "ChessmanGroup.h"
#include "Game.h"
#include "Stack.h"

namespace CIG
{
	class Player//: public CIGNamedObject<CIGRuleConfig::PLAYER>
	{
		private:
			Player();
		public:
			Player(CIGRuleConfig::PLAYER_NAMES p);
			Player(const Player& p);
			virtual ~Player();

			CIGRuleConfig::PLAYER_NAMES NAME;
			vector<Chessman> ownedChessmans;

			static bool wins();
			virtual void makeBestAction(OperationStack& op);
			//static const unsigned char color[CIGRuleConfig::PLAYER_NUM][3];
			//static const HBITMAP bitMap;
	};
}

#endif /*__PLAYER_H_*/

