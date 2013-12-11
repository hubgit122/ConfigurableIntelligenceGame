
#ifndef __CHESSMAN_LOCATION_BOARD__
#define __CHESSMAN_LOCATION_BOARD__

#include "ChessmanIndex.h"
#include "CIGRuleConfig.h"
#include "GUI.h"
#include "utilities.h"

namespace CIG
{
	class ChessmanIndexBoard
	{
	public:
		ChessmanIndexBoard();
		ChessmanIndexBoard(const ChessmanIndexBoard& clb);
		~ChessmanIndexBoard();

		void operator= (const ChessmanIndexBoard& clb);

		ChessmanIndex& operator[](PointOrVector p);

		ChessmanIndex* content;
	};
}





#endif  //__CHESSMAN_LOCATION_BOARD__