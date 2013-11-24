
#ifndef __CHESSMAN_LOCATION__
#define __CHESSMAN_LOCATION__

#include "utilities.h"
#include "CIGRuleConfig.h"

namespace CIG
{
	class ChessmanLocation
	{
	public:
		ChessmanLocation(void);
		ChessmanLocation(CIGRuleConfig::PLAYER_NAMES p, int i);
		ChessmanLocation(const ChessmanLocation& cl);
		virtual ~ChessmanLocation(void);

		CIGRuleConfig::PLAYER_NAMES player;
		int index;

		bool operator==(const ChessmanLocation& cl);
		void clear();
	};
}
#endif //__CHESSMAN_LOCATION__
