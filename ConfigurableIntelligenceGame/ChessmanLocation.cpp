#include "ChessmanLocation.h"
#include "GUI.h"
namespace CIG
{
	ChessmanLocation::ChessmanLocation(void)
	{
		clear();
	}

	ChessmanLocation::ChessmanLocation( const ChessmanLocation& cl ): player(cl.player), index(cl.index)
	{
	}


	ChessmanLocation::ChessmanLocation( CIGRuleConfig::PLAYER_NAMES p, int i ): player(p), index(i)
	{
	}

	ChessmanLocation::~ChessmanLocation(void)
	{
	}

	void ChessmanLocation::clear()
	{
		player = (CIGRuleConfig::PLAYER_NAMES)-1;
		index = -1;
	}

	bool ChessmanLocation::operator==( const ChessmanLocation& cl )
	{
		return (player==cl.player)&&(index==cl.index);
	}

}
