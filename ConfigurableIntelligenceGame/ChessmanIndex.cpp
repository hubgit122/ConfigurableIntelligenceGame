#include "stdafx.h"
#include "ChessmanIndex.h"
#include "GUI.h"
namespace CIG
{
	ChessmanIndex::ChessmanIndex(void)
	{
		clear();
	}

	ChessmanIndex::ChessmanIndex( const ChessmanIndex& ci ): player(ci.player), index(ci.index)
	{
	}


	ChessmanIndex::ChessmanIndex( CIGRuleConfig::PLAYER_NAMES p, int i ): player(p), index(i)
	{
	}

	ChessmanIndex::~ChessmanIndex(void)
	{
	}

	void ChessmanIndex::clear()
	{
		player = (CIGRuleConfig::PLAYER_NAMES) - 1;
		index = -1;
	}

	bool ChessmanIndex::operator==( const ChessmanIndex& ci )
	{
		return (player == ci.player) && (index == ci.index);
	}

	void ChessmanIndex::operator=( const ChessmanIndex& ci )
	{
		memcpy(this, &ci, sizeof(ChessmanIndex));
	}
}
