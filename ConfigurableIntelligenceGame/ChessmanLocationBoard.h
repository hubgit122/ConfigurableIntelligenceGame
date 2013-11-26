
#ifndef __CHESSMAN_LOCATION_BOARD__
#define __CHESSMAN_LOCATION_BOARD__

#include "ChessmanLocation.h"
#include "CIGRuleConfig.h"
#include "GUI.h"
#include "utilities.h"

namespace CIG
{
	class ChessmanLocationBoard
	{
	public:
		ChessmanLocationBoard();
		ChessmanLocationBoard(const ChessmanLocationBoard& clb);
		~ChessmanLocationBoard();

		void operator= (const ChessmanLocationBoard& clb);

		ChessmanLocation& operator[](PointOrVector p);

		ChessmanLocation* content;
	};
}





#endif  //__CHESSMAN_LOCATION_BOARD__