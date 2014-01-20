#include "stdafx.h"
#include "ChessmanIndexBoard.h"

namespace CIG
{
	void CIG::ChessmanIndexBoard::operator=( const ChessmanIndexBoard& clb )
	{
		if (!content)
		{
			if ((content = (ChessmanIndex*) malloc(sizeof(ChessmanIndex) << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2)) == 0)
			{
				GUI::inform("在生成ChessmanLocaionBoard时内存不足. ", true);
			}
		}

		memcpy(content, clb.content, sizeof(ChessmanIndex) << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanIndexBoard::ChessmanIndexBoard()
	{
		if ((content = (ChessmanIndex*) malloc(sizeof(ChessmanIndex) << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2)) == 0)
		{
			GUI::inform("在生成ChessmanLocaionBoard时内存不足. ", true);
		}

		memset(content, -1, sizeof(ChessmanIndex) << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanIndexBoard::ChessmanIndexBoard( const ChessmanIndexBoard& clb )
	{
		if ((content = (ChessmanIndex*) malloc(sizeof(ChessmanIndex) << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2)) == 0)
		{
			GUI::inform("在生成ChessmanLocaionBoard时内存不足. ", true);
		}

		memcpy(content, clb.content, sizeof(ChessmanIndex) << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanIndexBoard::~ChessmanIndexBoard()
	{
		if (content)
		{
			free(content);
		}
	}

	CIG::ChessmanIndex& ChessmanIndexBoard::operator[]( PointOrVector p )
	{
		return content[ (p[1] << CIGRuleConfig::INI_BOARD_WIDTH_LOG2) + p[0] ];
	}
}


