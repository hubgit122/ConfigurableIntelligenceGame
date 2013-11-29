#include "stdafx.h"
#include "ChessmanLocationBoard.h"

namespace CIG
{
	void CIG::ChessmanLocationBoard::operator=( const ChessmanLocationBoard& clb )
	{
		if (!content)
		{
			if ((content = (ChessmanIndex*) malloc(sizeof(ChessmanIndex)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2))==0)
			{
				GUI::inform("在生成ChessmanLocaionBoard时内存不足. ",true);
			}
		}
		memcpy(content, clb.content, sizeof(ChessmanIndex)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanLocationBoard::ChessmanLocationBoard()
	{
		if ((content = (ChessmanIndex*) malloc(sizeof(ChessmanIndex)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2))==0)
		{
			GUI::inform("在生成ChessmanLocaionBoard时内存不足. ",true);
		}
		memset(content,-1,sizeof(ChessmanIndex)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanLocationBoard::ChessmanLocationBoard( const ChessmanLocationBoard& clb )
	{
		if ((content = (ChessmanIndex*) malloc(sizeof(ChessmanIndex)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2))==0)
		{
			GUI::inform("在生成ChessmanLocaionBoard时内存不足. ",true);
		}
		memcpy(content, clb.content, sizeof(ChessmanIndex)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanLocationBoard::~ChessmanLocationBoard()
	{
		if (content)
		{
			free(content);
		}
	}

	CIG::ChessmanIndex& ChessmanLocationBoard::operator[]( PointOrVector p )
	{
		return content[ (p[1]<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2)+p[0] ];
	}
}


